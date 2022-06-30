#pragma once

#include "cppitertools/enumerate.hpp"
#include "cppitertools/range.hpp"

#include <Eigen/Core>

#include <functional>
#include <vector>


template<class T>
struct KalmanSmoother
{
    using DataTable = std::vector<std::vector<T>>;

    Eigen::VectorX<T> x;
    Eigen::MatrixX<T> P;
    Eigen::MatrixX<T> F;
    Eigen::MatrixX<T> H;
    Eigen::MatrixX<T> R;
    Eigen::MatrixX<T> Q;

    std::vector<Eigen::VectorX<T>> x_;
    std::vector<Eigen::VectorX<T>> _x;
    std::vector<Eigen::MatrixX<T>> P_;
    std::vector<Eigen::MatrixX<T>> _P;

    std::function<void(int, std::vector<T>)> function_beforeKFcorrect;
    std::function<void(int, std::vector<T>)> function_afterKFcorrect;

    KalmanSmoother(int dynamParams, int measureParams, int controlParams = 0);

    DataTable forwardPass(DataTable & dataSeries);
    DataTable backwardPass();

private:

    int m_dynamParams;
    int m_measureParams;
    int m_controlParams;

    std::vector<std::vector<T>> zk;
};

template<class T>
inline KalmanSmoother<T>::KalmanSmoother(int dynamParams, int measureParams, int controlParams)
{
    m_dynamParams   = dynamParams;
    m_measureParams = measureParams;
    m_controlParams = controlParams;

    x = Eigen::VectorX<T>(dynamParams);
    P = Eigen::MatrixX<T>(dynamParams, dynamParams);
    F = Eigen::MatrixX<T>(dynamParams, dynamParams);
    H = Eigen::MatrixX<T>(measureParams, dynamParams);
    R = Eigen::MatrixX<T>(measureParams, measureParams);
    Q = Eigen::MatrixX<T>(dynamParams, dynamParams);
}

template<class T>
inline std::vector<std::vector<T>>
  KalmanSmoother<T>::forwardPass(DataTable & dataSeries)
{
    for (auto & data : dataSeries) {
        if (data.size() != m_measureParams)
            throw std::length_error("One of the data point's dimension is inadequate.");
    }

    x_.clear();
    x_.resize(dataSeries.size() + 1, x);
    _x.clear();
    _x.resize(dataSeries.size() + 1, x);
    P_.clear();
    P_.resize(dataSeries.size() + 1, P);
    _P.clear();
    _P.resize(dataSeries.size() + 1, P);

    for (auto && [idx, data] : iter::enumerate(dataSeries)) {
        const int i = idx + 1;

        Eigen::Map<Eigen::VectorX<T>> z(data.data(), m_measureParams);

        // Kalman predict
        _x[i] = F * x_[i - 1];
        _P[i] = F * P_[i - 1] * F.transpose() + Q;

        // Kalman correct
        auto K = _P[i] * H.transpose() * (R + H * _P[i] * H.transpose()).inverse();
        auto I = Eigen::MatrixX<T>::Identity(m_dynamParams, m_dynamParams);
        x_[i]  = _x[i] + K * (z - H * _x[i]);
        P_[i]  = (I - K * H) * _P[i];
    }

    DataTable result;
    result.reserve(x_.size());
    for (auto & x : x_) {
        std::vector<T> temp;
        for (auto value : x) {
            temp.push_back(value);
        }
        result.push_back(temp);
    }

    return result;
}

template<class T>
inline std::vector<std::vector<T>>
  KalmanSmoother<T>::backwardPass()
{
    const int n = x_.size();

    std::vector<Eigen::VectorX<T>> xs;
    xs.resize(n, x_[n - 1]);
    std::vector<Eigen::MatrixX<T>> Ps;
    Ps.resize(n, P_[n - 1]);

    for (auto i : iter::range(n - 2, -1, -1)) {
        // Ps[i] = F * P_[i] * F.transpose() + Q;

        // auto K = P_[i] * F.transpose() * Ps[i].inverse();
        // xs[i] += K * (x_[i + 1] - F * _x[i]);
        // Ps[i] += K * (P_[i + 1] - P_[i]) * K.transpose();

        auto C = P_[i] * F.transpose() * _P[i + 1].inverse();

        xs[i] += x_[i] + C * (xs[i + 1] - _x[i + 1]);
        Ps[i] += P_[i] + C * (Ps[i + 1] - _P[i + 1]) * C.transpose();
    }

    DataTable result;
    result.reserve(xs.size());
    for (auto & x : xs) {
        std::vector<T> temp;
        for (auto value : x) {
            temp.push_back(value);
        }
        result.push_back(temp);
    }

    return result;
}
