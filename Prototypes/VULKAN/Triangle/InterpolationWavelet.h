#pragma once
#include <array>
#include <type_traits>

template <class T> struct ValueTraits {

	static constexpr T zero() {
		if constexpr (std::is_base_of_v<Eigen::DenseBase<T>, T>) {
			return T::Zero();
		}
		else {
			return 0;
		}
	}
};

template <class T, std::size_t N> struct ValueTraits<std::array<T, N>> {
	static constexpr std::array<T, N> zero() {
		std::array<T, N> out{};
		out.fill(0);
		return out;
	}
};

template <int I, class... Ts> constexpr decltype(auto) get(Ts &&... ts) {
	return std::get<I>(std::forward_as_tuple(ts...));
}

template <int I>
auto InterpolateNthArgument = [](auto fun, auto interpolation) {
	return [=](auto... x) mutable {
		// static_assert(std::is_invocable<decltype(fun), decltype(x)...>::value,
			//        "Invalid aguments");

		auto foo = [fun, x...](int i) mutable {
			get<I>(x...) = i;
			return fun(x...);
		};

		auto xI = get<I>(x...);
		return interpolation(foo)(xI);
	};
};

template <int I, class Fun, class... Interpolation>
auto InterpolationDimWise_impl(Fun fun, Interpolation... interpolation) {

	if constexpr (I == sizeof...(interpolation)) {
		return fun;
	}
	else {
		auto interpol = get<I>(interpolation...);
		return InterpolationDimWise_impl<I + 1>(
			InterpolateNthArgument<I>(fun, interpol), interpolation...);
	}
}

template <int I, int... J, class Fun, class... Interpolation>
auto InterpolationDimWise_impl2(Fun fun, Interpolation... interpolation) {

	if constexpr (I == sizeof...(J)) {
		return fun;
	}
	else {
		constexpr int K = get<I>(J...);
		auto interpol = get<K>(interpolation...);
		return InterpolationDimWise_impl2<I + 1, J...>(
			InterpolateNthArgument<K>(fun, interpol), interpolation...);
	}
}

template <class... Interpolation>
auto InterpolationDimWise(Interpolation... interpolation) {
	return [=](auto fun) {
		return InterpolationDimWise_impl<0>(fun, interpolation...);
	};
}

template <int... J, class... Interpolation>
auto InterpolationDimWise2(Interpolation... interpolation) {
	static_assert(
		sizeof...(J) == sizeof...(Interpolation),
		"The number of interpolations must match the number order indices");

	return [=](auto fun) {
		return InterpolationDimWise_impl2<0, J...>(fun, interpolation...);
	};
}

auto ConstantInterpolation = [](auto fun) {
	using ValueType = std::remove_reference_t<decltype(fun(0))>;
	return [=](auto x) mutable -> float {
		int ix = (int)round(x);
		return fun(ix);
	};
};

auto LinearInterpolation = [](auto fun) {
	// The first two lines are here because Eigen types do
	using ValueType = std::remove_reference_t<decltype(fun(0))>;
	ValueType zero = ValueTraits<ValueType>::zero();
	return [=](auto x) mutable -> float {
		int ix = (int)floor(x);
		float wx = x - ix;

		return (wx != 0 ? wx * fun(ix + 1) : zero) +
			(wx != 1 ? (1 - wx) * fun(ix) : zero);

	};
};


/*
 * Domain inteprolation - Interpolates function values only for pointS in the
 * domain
 *
 * \tparam Interpolation This is any function satisfying concept Interpolatiobn
 * \tparam Domain This is bool valued function on integers returning true for
 points inside of the domain and false otherwise
 * \param interpolation Interpolation to use.
 * \param domain Function indicating domain.
 */
template <class Interpolation, class Domain>
auto DomainInterpolation(Interpolation interpolation, Domain domain) {
	return [=](auto fun) mutable {

		// The function `dom_fun` collects function values and weights inside of
		// the domain
		auto dom_fun = [=](auto... x) mutable -> glm::vec2 {
			glm::vec2 val;
			if (domain(x...) == true) {
				val = { fun(x...), 1.0 };
			}
			else {
				val = { 0.0, 0.0 };
			}
			return val;
		};

		// Interpolates `dom_fun`
		auto int_fun = interpolation(dom_fun);

		return [=](auto... x) mutable {
			auto val = int_fun(x...);
			//float f = val[0];
			//float w = val[1];
			return val; // f / w;// w != 0.0 ? f / w : 0.0;
		};
	};
}