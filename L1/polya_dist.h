//
// Created by mhq on 13/03/23.
//

#pragma once

#include <random>
#include <tuple>

//template<class IntType = int>
//struct polya_1_distribution {
//    static_assert(std::is_integral<IntType>::value,
//                  "result_type must be an integral type");
//
//    /** The type of the range of the distribution. */
//    using result_type = IntType;
//
//    /** Parameter type. */
//    struct param_type {
//        using distribution_type = polya_1_distribution<IntType>;
//        IntType black_n, red_n, c;
//        IntType size;
//        double p0; // precalculated for x = 0
//
//        param_type(IntType black_n, IntType red_n, IntType c, IntType size)
//                : black_n{black_n}, red_n{red_n}, c{c}, size{size}, p0{1} {
//            for (int i = 0; i < p.size; i++) {
//                p0 *= (p.red_n + i * p.c) / (p.black_n + p.red_n + i * p.c);
//            }
//        }
//
//        friend bool
//        operator==(const param_type &lhs, const param_type &rhs) {
//            return std::tie(lhs.black_n, lhs.red_n, lhs.c, lhs.size)
//                == std::tie(rhs.black_n, rhs.red_n, rhs.c, rhs.size);
//        }
//
//        friend bool
//        operator!=(const param_type &lhs, const param_type &rhs) { return !(lhs == rhs); }
//    };
//
//    polya_1_distribution(IntType black_n, IntType red_n, IntType c, IntType size) : m_param{black_n, red_n, c, size} {
//    }
//
//    /**
//     * @brief Resets the distribution state.
//     */
//    void reset() { }
//
//    /**
//     * @brief Returns the parameter set of the distribution.
//     */
//    param_type param() const { return m_param; }
//    /**
//     * @brief Sets the parameter set of the distribution.
//     * @param _param The new parameter set of the distribution.
//     */
//    void param(const param_type& _param) { m_param = _param; }
//    /**
//     * @brief Returns the greatest lower bound value of the distribution.
//     */
//    result_type min() const { return {0}; }
//
//    /**
//     * @brief Returns the least upper bound value of the distribution.
//     */
//    result_type max() const { return m_param.size; }
//
//    /**
//       * @brief Generating functions.
//       */
//    template<typename UniformRandomNumberGenerator>
//	result_type operator()(UniformRandomNumberGenerator& urng)
//	{ return this->operator()(urng, m_param); }
//
//    template<typename UniformRandomNumberGenerator>
//	result_type operator()(UniformRandomNumberGenerator& urng, const param_type& p) {
//        constexpr std::uniform_real_distribution<> dis(0.0, 1.0);
//        constexpr auto alpha = [&p](result_type x) {
//            return (p.size + 1 - x)
//                * (p.black_n + (x - 1) * p.c)
//                / (x * (p.red_n + (p.size - x) * p.c));
//        };
//
//        double r = dis(urng);
//        double probab = p.p0;
//        for (IntType x = 0; x < p.size; ++x, probab *= alpha(x)) {
//            r -= probab;
//            if (r < 0) {
//                return x;
//            }
//        }
//        return p.size;
//    }
//
//    /**
//     * @brief Return true if two discrete distributions have the same
//     *        parameters.
//     */
//    friend bool operator==(const polya_1_distribution& lhs, const polya_1_distribution& rhs)
//    { return lhs.m_param == rhs.m_param; }
//
//    /**
//     * @brief Inserts a %polya_1_distribution random number distribution
//     * @p x into the output stream @p os.
//     *
//     * @param os An output stream.
//     * @param x  A %polya_1_distribution random number distribution.
//     *
//     * @returns The output stream with the state of @p x inserted or in
//     * an error state.
//     */
//    template<typename IntType1>
//	friend std::ostream&
//	operator<<(std::ostream& os,
//		   const polya_1_distribution<IntType1>& x) {
//        return os
//            << x.m_param.red_n << ' '
//            << x.m_param.black_n << ' '
//            << x.m_param.c << ' '
//            << x.m_param.size;
//    }
//
//    /**
//     * @brief Extracts a %polya_1_distribution random number distribution
//     * @p x from the input stream @p is.
//     *
//     * @param is An input stream.
//     * @param x A %polya_1_distribution random number
//     *            generator engine.
//     *
//     * @returns The input stream with @p x extracted or in an error
//     *          state.
//     */
//    template<typename IntType1>
//	friend std::istream&
//	operator>>(std::istream& is,
//		   polya_1_distribution<IntType1>& x) {
//        return is
//            >> x.m_param.red_n
//            >> x.m_param.black_n
//            >> x.m_param.c
//            >> x.m_param.c
//            >> x.m_param.size;
//    }
//
//    param_type m_param;
//};

template <class IntType = int>
std::discrete_distribution<IntType> polya_1_distribution(IntType black_n, IntType red_n, IntType c, IntType size) {
    std::vector<double> probabilities;
    probabilities.reserve(size + 1);
    double p0 = 1;
    for (int i = 0; i < size; i++) {
        p0 *= (red_n + i * c);
        p0 /= (black_n + red_n + i * c);
    }
    double p = p0;
    probabilities.push_back(p);
    for (int x = 1; x <= size; ++x) {
        p *= (size + 1 - x) * (black_n + (x - 1) * c);
        p /= x * (red_n + (size - x) * c);
        probabilities.push_back(p);
    }
    return std::discrete_distribution<IntType>(probabilities.begin(), probabilities.end());
}
