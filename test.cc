#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>
#include <ranges>
#include <boost/multiprecision/cpp_int.hpp>
#include <csignal>

void swap_reverse(std::string& text, size_t begin, size_t end) {
	// size_t begin = 1;
	// size_t end = text.size();
	while (true) {
		if (end - begin == 0) break;

		std::swap(text[begin - size_t{1}], text[end - size_t{1}]);

		if (end - begin == 1) break;

		begin += 1, end -= 1;
	}
}

bool isPrime(std::size_t n) noexcept
{
	if (n <= 1)
	return false;

	for (size_t i = 2; i < size_t(n); i++)
	{
		if(size_t(n) % i == 0)
		return false;
	}

	return true;
}

std::string produce_new_string(std::size_t i)
{
	// auto N_str = "9_95_51_16_6"s;
	auto i_str = std::to_string(i);
	return '9' + i_str + "95" + i_str + "51" + i_str + "16" + i_str + '6';
}

using namespace std::string_literals;

std::uint64_t ascii_warpping_add(std::uint64_t lhs, size_t rhs) {
	auto MAX = std::uint64_t('z');
	auto result = lhs + rhs;
	
	if (result > MAX)
		return MAX - result;
	else
		return result;
}

void forward_shift_string(std::string& S, std::vector<std::array<std::uint64_t, 3>> queries) {
	std::uint64_t new_alphabet_index{};
	for (auto &&query : queries)
	{
		for (std::uint64_t i = query[0] - 1; i <= query[1] - 1; i++)
		{
			new_alphabet_index = ascii_warpping_add(std::uint64_t(S[i]), query[2]);
			S[i] = char(std::uint8_t(new_alphabet_index));
		}
	}
}

void signal_handler(int signal) {
	if (signal == SIGINT) {
		std::cout << "Signal Interupt called!\n";
	}
}

int main() {
	boost::multiprecision::cpp_int N{};

	signal(SIGINT, signal_handler);
	
	for (size_t i = 10001; i <= 11000; i++)
	{
		if (isPrime(i))
		{
			N = boost::multiprecision::cpp_int{produce_new_string(i)};
			if (i == 10303) {
				std::raise(SIGINT);
				std::cout << "Continuing Execution\n";
			}
			std::cout << "P: " << i << " N: " << N << " R: " << N%11 << '\n';
		}
	}
	
	// std::uint64_t N, Q{};
	// std::string S{};
	// std::vector<std::array<std::uint64_t, 3>> queries{};

	// std::cin >> N >> Q;
	// std::cin >> S;
	// queries.reserve(Q);

	// std::array<std::uint64_t, 3> query{};
	// for (size_t i = 0; i < Q; i++)
	// {
	// 	std::cin >> query[0] >> query [1] >> query[2];
	// 	queries.push_back(query);
	// }
	// forward_shift_string(S, queries);
	// std::cout << S;
}
