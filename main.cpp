#include <deque>
#include <iostream>
#include <stack>
#include <vector>

class Vocabulary {
 public:
  Vocabulary();
  void AddWord(const std::string& word);
  std::string PredictWord(const std::string& prefix);
  std::string PredictPrevWordWithNewSymbols(const std::string& prefix);

 private:
  static const char kAlphabetStartSymbol = 'a';
  static const char kAlphabetEndSymbol = 'z';
  static const char kTermSymbol = '#';
  static const size_t kAlphabetSize = kAlphabetEndSymbol - kAlphabetStartSymbol + 2;
  struct Node {
    Node* children[kAlphabetSize];
    size_t max_n = 0;
  };
  std::deque<Node> bor_;
  std::string prev_prefix_;
  const Node* prev_node_ = nullptr;
  static size_t SymbolNumber(char symbol);
  static char SymbolFromNumber(size_t number);
  const Node* FindNodeInBor(const std::string& prefix, const Node* start_node = nullptr);
  std::string PredictWord(const std::string& prefix, const Node* start_node);
};

Vocabulary::Vocabulary() {
  bor_.emplace_back();
}

size_t Vocabulary::SymbolNumber(char symbol) {
  return symbol != kTermSymbol ? symbol - kAlphabetStartSymbol : kAlphabetSize - 1;
}

char Vocabulary::SymbolFromNumber(size_t number) {
  return number < kAlphabetSize - 1 ? static_cast<char>(kAlphabetStartSymbol + number) : kTermSymbol;
}

void Vocabulary::AddWord(const std::string& word) {
  Node* current = &*bor_.begin();
  std::stack<Node*> parent_stack;
  for (char symbol : word) {
    if (current->children[SymbolNumber(symbol)] == nullptr) {
      current->children[SymbolNumber(symbol)] = &bor_.emplace_back();
    }
    parent_stack.push(current);
    current = current->children[SymbolNumber(symbol)];
  }
  if (current->children[SymbolNumber(kTermSymbol)] == nullptr) {
    current->children[SymbolNumber(kTermSymbol)] = &bor_.emplace_back();
  }
  parent_stack.push(current);
  current = current->children[SymbolNumber(kTermSymbol)];
  ++current->max_n;
  while (current != &*bor_.begin()) {
    Node* parent = parent_stack.top();
    parent_stack.pop();
    parent->max_n = std::max(parent->max_n, current->max_n);
    current = parent;
  }
}

std::string Vocabulary::PredictWord(const std::string& prefix) {
  std::string result = PredictWord(prefix, &*bor_.begin());
  prev_prefix_ = prefix;
  return result;
}

std::string Vocabulary::PredictPrevWordWithNewSymbols(const std::string& prefix) {
  std::string result = prev_prefix_ + PredictWord(prefix, prev_node_);
  prev_prefix_ = prefix;
  return result;
}

const Vocabulary::Node* Vocabulary::FindNodeInBor(const std::string& prefix, const Node* start_node) {
  const Node* current = start_node != nullptr ? start_node : &*bor_.begin();
  for (char symbol : prefix) {
    if (current->children[SymbolNumber(symbol)] == nullptr) {
      return nullptr;
    }
    current = current->children[SymbolNumber(symbol)];
  }
  return current;
}

std::string Vocabulary::PredictWord(const std::string& prefix, const Node* start_node) {
  std::string result = prefix;
  const Node* current = FindNodeInBor(prefix, start_node);
  prev_node_ = current;
  if (current == nullptr) {
    return result;
  }
  while (true) {
    size_t next_symbol_n = 0;
    while (current->children[next_symbol_n] == nullptr || current->children[next_symbol_n]->max_n != current->max_n) {
      ++next_symbol_n;
    }
    char next_symbol = SymbolFromNumber(next_symbol_n);
    if (next_symbol == kTermSymbol) {
      break;
    }
    result += next_symbol;
    current = current->children[next_symbol_n];
  }
  return result;
}


int main() {
  Vocabulary vocabulary;
  std::string rules = "Введите один из запросов:\n exit - выйти\n add_word - добавить слово в словарь\n add_text - добавить несколько слов в словарь\n predict - предсказать слово\n predict_prev_with_new_symbols - предсказать предыдущее слово, но с добавлением новых символов в префикс\n";
  std::cout << rules;
  while (true) {
    std::string input;
    std::cin >> input;
    if (input == "exit") {
      std::cout << "Программа завершена\n";
      break;
    }
    if (input == "add_word") {
      std::cout << "Введите слово для добавления\n";
      std::cin >> input;
      vocabulary.AddWord(input);
      std::cout << "Слово успешно добавлено\n";
      continue;
    }
    if (input == "add_text") {
      std::cout << "Введите последовательность слов для добавления:\n";
      std::cin.ignore();
      std::getline(std::cin, input);
      size_t word_start_n = 0;
      while (word_start_n != input.size()) {
        size_t word_end_n = input.find(' ', word_start_n);
        word_end_n = word_end_n != std::string::npos ? word_end_n : input.size();
        vocabulary.AddWord(input.substr(word_start_n, word_end_n - word_start_n));
        word_start_n = input.find_first_not_of(' ', word_end_n);
        word_start_n = word_start_n != std::string::npos ? word_start_n : input.size();
      }
      std::cout << "Слова успешно добавлены\n";
      continue;
    }
    if (input == "predict") {
      std::cout << "Введите префикс слова для предсказания:\n";
      std::cin >> input;
      std::cout << "Предсказанное слово: " << vocabulary.PredictWord(input) << '\n';
      continue;
    }
    if (input == "predict_prev_with_new_symbols") {
      std::cout << "Введите продолжение префикса предыдущего слова:\n";
      std::cin >> input;
      std::cout << "Предсказанное слово: " << vocabulary.PredictPrevWordWithNewSymbols(input) << '\n';
      continue;
    }
    std::cout << "Запрос не распознан\n";
    std::cout << rules;
  }
  return 0;
}
