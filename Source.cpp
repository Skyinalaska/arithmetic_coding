#include <iostream>
#include <map>
#include <string>
#include <vector>

std::vector<std::pair<char, int> > frequency_symbol(std::string input, int& count_of_symbol)
{
	input.push_back('\0');
	std::map<char, int> frequency;
	char symbol;
	for (int i = 0; i < input.length(); i++)
	{
		frequency[input[i]]++;
		count_of_symbol++;
	}
	std::multimap<int, char> freq;
	for (auto it = frequency.begin(); it != frequency.end(); ++it)
	{
		freq.insert(std::pair<int, char>(it->second, it->first));
	}
	std::vector<std::pair<char, int> > cum_freq(freq.size()+1);
	int i = 1;
	cum_freq[0] = std::pair<char, int>('*', 0);
	int last = 0;
	for (auto it = freq.rbegin(); it != freq.rend(); ++it)
	{
		cum_freq[i]=std::pair<char, int>(it->second, it->first+last);
		last = it->first+last;
		i++;
	}
	return cum_freq;

}

int index_of_symbol(char symbol, std::vector<std::pair<char, int> > cum_freq)
{
	for (int i = 0; i < cum_freq.size(); i++)
	{
		if(cum_freq[i].first==symbol) 
			return i;
	}
}

void bits_plus_follow(bool bit, int bits_to_follow, std::vector<bool>& output)
{
	output.push_back(bit);
	while (bits_to_follow > 0)
	{
		output.push_back(!bit);
		bits_to_follow -= 1;
	}
}

std::vector<bool> encode(std::string input, std::vector<std::pair<char, int> > cum_freq, int count_of_symbol)
{
	input.push_back('\0');
	const int length = input.length();
	unsigned short int* h = new unsigned short int[input.length()];
	unsigned short int* l = new unsigned short int[input.length()];

	l[0] = 0;
	h[0] = 65535;

	const unsigned short int first_fourth = h[0] / 4 + 1;
	const unsigned short int middle  = 2 * first_fourth;
	const unsigned short int third_fourth = 3 * first_fourth;

	int bits_to_foll = 0;
	std::vector<bool> output;
	for (int i = 0; i < input.length();)
	{
		char tmp_symbol = input[i];
		int index = index_of_symbol(input[i], cum_freq);
		i++;
		unsigned int range = h[i - 1] - l[i - 1] + 1;
		l[i] = l[i - 1] + (range * cum_freq[index - 1].second) / count_of_symbol;
		h[i] = l[i - 1] + (range * cum_freq[index].second) / count_of_symbol - 1;
		for (;;)
		{
			if (h[i] < middle)
			{
				bits_plus_follow(0, bits_to_foll, output);
				bits_to_foll = 0;
			}
			else if (l[i] >= middle)
			{
				bits_plus_follow(1, bits_to_foll, output);
				bits_to_foll = 0;
				//Нормализуем интервал, уменьшая значения границ, если они находятся во второй части рабочего интервала
				l[i] -= middle;
				h[i] -= middle;
			}
			else if (l[i] >= first_fourth && h[i] < third_fourth)
			{
				// Если текущий интеpвал  содеpжит сеpедину исходного, то вывод еще одного обpатного бита позже, а сейчас убpать общую часть
				bits_to_foll++;
				l[i] -= first_fourth;
				h[i] -= first_fourth;
			}
			else
			{
				break;
			}
			//расширяем рабочий интервал
			l[i] = 2 * l[i];
			h[i] = 2 * h[i] + 1;
		}

	}      
	for (int i = 0; i < output.size(); i++)
		std::cout << output[i] << ' ';
	return output;
}


unsigned short int get_value(std::vector<bool> code)
{
	unsigned short int value = 0;
	for (int i = 0; i < 16; i++)
	{
		if (i == code.size())
		{
			return value;
		}
		if (true == code[i])
		{
			value = value | (1 << (15 - i));
		}
	}
	return value;
}

unsigned short int new_bit(std::vector<bool> code, int & code_index, bool& isCodeEnded)
{
	if (code_index < code.size())
	{
		if (code[code_index++] == 0)
			return 0;
		else
			return 1;
	}
	else
		if (isCodeEnded)
		{
			return 0;
		}
		else
		{
			isCodeEnded = 1;
			return 1;
		}
}

std::string decode(std::vector<bool> code, std::vector<std::pair<char, int> > cum_freq, int count_of_symbol)
{
	unsigned short int* h = new unsigned short int[code.size()];
	unsigned short int* l = new unsigned short int[code.size()];

	l[0] = 0;
	h[0] = 65535;

	const unsigned short int first_fourth = h[0] / 4 + 1;
	const unsigned short int middle = 2 * first_fourth;
	const unsigned short int third_fourth = 3 * first_fourth;
	std::string output;
	unsigned short int value = get_value(code);
	int code_index = 16;
	bool is_code_ended = 0;
	for (int i = 1;; ++i)
	{
		unsigned int range = h[i - 1] - l[i - 1] + 1;
		unsigned int cum = ((value - l[i - 1] + 1) * count_of_symbol - 1) / range;

		int index;
		for (index = 1; cum_freq[index].second <= cum; ++index)
		{ }

		l[i] = l[i - 1] + (range * cum_freq[index-1].second) / count_of_symbol;
		h[i] = l[i - 1] + (range * cum_freq[index].second) / count_of_symbol - 1;

		if (cum_freq[index].first == '\0')
		{
			return output;
		}
		output.push_back(cum_freq[index].first);
		std::cout << cum_freq[index].first;
		for (;;)
		{
			if (h[i] >= middle)
			{
				if (l[i] >= middle)
				{
					value -= middle; l[i] -= middle; h[i] -= middle;
				}
				else
					if (l[i] >= first_fourth && h[i] < third_fourth)
					{
						value -= first_fourth; l[i] -= first_fourth; h[i] -= first_fourth;
					}
					else
					{
						break;
					}
			}
			
			l[i] = 2 * l[i];
			h[i] = 2 * h[i] + 1;
			value = 2 * value + new_bit(code, code_index, is_code_ended);
		}
	}
}

int main()
{
	std::string input = "adfffadfa";
	int count_of_symbol = 0;
	std::vector<std::pair<char, int> > cum_freq = frequency_symbol(input, count_of_symbol);
	std::vector<bool> code = encode(input, cum_freq, count_of_symbol);
	decode(code, cum_freq, count_of_symbol);

}