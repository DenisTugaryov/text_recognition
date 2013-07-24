#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <stack>
#include <cctype>

const int ALPHABET_SIZE = 26;
const int BIGRAMM_SIZE = ALPHABET_SIZE * ALPHABET_SIZE;

struct First_char_frequency
{
  std::map<char, int> _first_char_number;
  int _all_first_char_number;
};

void get_first_char_frequency(First_char_frequency & fcf)
{
  fcf._first_char_number.clear();
  fcf._all_first_char_number = 0;

  std::ifstream word_file("word_list.txt");
  std::string line;
  
  while (word_file.good())
  {
    getline(word_file, line);
    if (isalpha(line[0]))
    {
      fcf._first_char_number[line[0]]++;
      fcf._all_first_char_number++;
    }
  }
}

void save_first_char_frequency(const First_char_frequency & fcf)
{
  std::ofstream output_file_first_char("first_char_frequency.txt");
  output_file_first_char << fcf._all_first_char_number << std::endl;
  for (std::map<char, int>::const_iterator it = fcf._first_char_number.begin(); it != fcf._first_char_number.end(); ++it)
    output_file_first_char << (*it).first << "\t" << (*it).second << std::endl;
}

struct Bigramms_frequency
{
  std::map<std::string, int> _bigramms_number;
  int _all_bigramms_number;
};

void get_bigramms_frequency(Bigramms_frequency & bf)
{
  bf._bigramms_number.clear();
  bf._all_bigramms_number = 0;

  std::ifstream word_file("word_list.txt");
  std::string line;

  while (word_file.good())
  {
    getline(word_file, line);
    std::string str_tmp;
    for (int i = 0; i < line.length() - 1; ++i)
    {
      if (line.length() > 1)
      {
        str_tmp = line.substr(i, 2);
        if (isalpha(str_tmp[0]) &&  isalpha(str_tmp[1]))
        {
          bf._bigramms_number[str_tmp]++;
          bf._all_bigramms_number++;
        }
      }
    }
  }
}

void save_bigramms_frequency(const Bigramms_frequency & bf)
{
  std::ofstream output_file_bigramms("bigramms_frequency.txt");
  output_file_bigramms << bf._all_bigramms_number << std::endl;
  for (std::map<std::string, int>::const_iterator it = bf._bigramms_number.begin(); it != bf._bigramms_number.end(); ++it)
    output_file_bigramms << (*it).first << "\t" << (*it).second << std::endl;

}

void get_prob_recognizer_matrix(std::map<std::string, double> & transition_map)
{
  std::ifstream transition_prob_file("prob_recognizer_matrix.txt");
  int test_number = 0;
  transition_prob_file >> test_number;
  for (char i = 'a'; i <= 'z' ; ++i)
  {
    for (char j = 'a'; j <= 'z'; ++j)
    {
      int recognition_number = 0;
      transition_prob_file >> recognition_number;
      std::string tmp_str = "aa";
      tmp_str[0] = i;
      tmp_str[1] = j;
      transition_map[tmp_str] = 1.0 * recognition_number / test_number;
    }
  }
}

void my_print(const std::vector<double> & v)
{
  for (int i = 0; i < ALPHABET_SIZE; ++i)
    std::cout << v[i] << "\t";
  std::cout << std::endl;
}

void my_print(const std::vector<char> & v)
{
  for (int i = 0; i < ALPHABET_SIZE; ++i)
    std::cout << v[i] << "\t";
  std::cout << std::endl;
}

int main()
{
  First_char_frequency fcf;
  Bigramms_frequency bf;

  get_first_char_frequency(fcf);
  get_bigramms_frequency(bf);

  std::map<std::string, double> recognizer;
  get_prob_recognizer_matrix(recognizer);

  std::ifstream input_word("input_word.txt");
  int char_number = 0;
  input_word >> char_number;
  std::string word;
  input_word >> word;

  std::cout << "input_word = " << word << std::endl;

  std::vector<double> empty_double_vector(ALPHABET_SIZE, 0.0);
  std::vector<std::vector<double> > delta(char_number, empty_double_vector);

  int simbol_counter = 0;
  for (char i = 'a'; i <= 'z' ; ++i)
  {
    std::string str_tmp = "aa";
    str_tmp[0] = i;
    str_tmp[1] = word[0];
    delta[0][simbol_counter] = fcf._first_char_number[i] * recognizer[str_tmp];
    simbol_counter++;
  }

  std::vector<char> empty_char_vector(ALPHABET_SIZE, 0.0);
  std::vector<std::vector<char> > psi(char_number, empty_char_vector);

  for (int i = 1; i < char_number; ++i)
  {
    simbol_counter = 0;
    for (char j = 'a'; j <= 'z'; ++j)
    {
      double max = 0;
      char arg_max = 'a';
      int counter = 0;
      for (char ch = 'a'; ch <= 'z'; ++ch)
      {
        std::string str_tmp = "aa";
        str_tmp[0] = ch;
        str_tmp[1] = j;
        if (delta[i-1][counter] * bf._bigramms_number[str_tmp] > max)
        {
          max = delta[i-1][counter] * bf._bigramms_number[str_tmp];
          arg_max = ch;
        }
        ++counter;
      }

      std::string str_tmp = "aa";
      str_tmp[0] = j;
      str_tmp[1] = word[i];
      delta[i][simbol_counter] = max * recognizer[str_tmp];
      psi[i][simbol_counter] = arg_max;
      simbol_counter++;
    }
  }
  
  double max = 0;
  char arg_max = 'a';
  int counter = 0;
  for (char ch = 'a'; ch <= 'z'; ++ch)
  {
    if (delta[char_number - 1][counter] > max)
    {
      max = delta[char_number - 1][counter];
      arg_max = ch;
    }
    ++counter;
  }

  std::map<char, int> char_number_in_alphabet;
  int my_counter = 0;
  for (char i = 'a'; i <= 'z'; ++i)
  {
    char_number_in_alphabet[i] = my_counter++;
  }

  std::stack<char> result;
  result.push(arg_max);
  char q = psi[char_number - 1][char_number_in_alphabet[arg_max]];
  for (int i = 0; i < char_number - 1; ++i)
  {
    result.push(q);
    char p = psi[char_number - i - 2][char_number_in_alphabet[q]];
    q = p;
  }

  std::cout << "result == ";
  for (int i = 0; i < char_number; ++i)
  {
    std::cout << result.top();
    result.pop();
  }
  std::cout << std::endl;

  for (int i = 0; i < char_number; ++i)
    my_print(delta[i]);

  for (int i = 1; i < char_number; ++i)
    my_print(psi[i]);
  
  return 0;
}
