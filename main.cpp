#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <queue>
#include <cstdio>

using namespace std;

class knot//Объекты этого класса представляют из себя узлы двоичного дерева, хранящие символ и количество его вхождений в текст
{
    char key;
    unsigned long long k;
    knot *left, *right;

public:

    knot(const char& letter = '0', unsigned long long amount = 0, knot* l = nullptr, knot* r = nullptr)
    {
        key = letter;
        k = amount;
        left = l;
        right = r;
    }

    ~knot()
    {
        left = nullptr;
        right = nullptr;
    }

    knot(const knot& leaf)
    {
        key = leaf.key;
        k = leaf.k;
        left = leaf.left;
        right = leaf.right;
    }

    knot operator = (const knot& leaf)
    {
        if (this != &leaf)
        {
            key = leaf.key;
            k = leaf.k;
            left = leaf.left;
            right = leaf.right;
        }
        return *this;
    }

    bool operator () (const knot& first, const knot& second)//Скобки используются в priority queue в качестве оператора сравнения
    {                                                       //Так как используется специфический класс их необходимо перегрузить для него
        return first.k >= second.k;
    }

    knot* join (knot new_elem)//Создает новый элемент: "предка" для узлов с суммой их "частот"
    {
        return new knot( '0' , new_elem.k + k, new knot(new_elem), this);
    }

    void coding(knot* tree, string code, string* code_table)//Заполняет таблицу кодов Хаффмана, последовательно переходя от "предка" к "потомку"
    {
        if (tree == nullptr)
            return;
        if (tree->left == nullptr && tree->right == nullptr)
            code_table[tree->key] = code;
        coding(tree->left, code + "0", code_table);
        coding(tree->right, code + "1", code_table);
    }

    void decode(unsigned long long *alphabet, FILE* input, FILE* output)
    {
        knot* runner = this;//Создаем бегунок,он будет проверять узлы дерева
        char symbol, letter;
        while (!feof(input))
        {
            letter = fgetc(input);
            if(!feof(input))
                for(int i = 7; i >= 0; i--)
                {
                    if(((letter >> i) & 1) == 1)//Смотрим на биты символа, в зависимости от значения идем либо по правой, либо по левой ветке
                    {
                        if (runner -> right == nullptr)//Если дошли до листа записываем символ
                        {
                            symbol = runner -> key;
                            if(alphabet[symbol] > 0)
                            {
                                alphabet[symbol]--;
                                fputc(symbol, output);
                                runner = this -> right;
                            }
                        }
                        else
                            runner = runner -> right;
                    }
                    else
                    {
                        if (runner -> left == nullptr)
                        {
                            symbol = runner -> key;
                            if(alphabet[symbol] > 0)
                            {
                                alphabet[symbol]--;
                                fputc(symbol, output);
                                runner = this -> left;
                            }
                        }
                        else
                            runner = runner -> left;
                    }
                }
        }
    }

    friend ostream& operator << (ostream& st, const knot& num)
    {
        st << num.key << ":" << num.k;
        return st;
    }

};

void decoder (const char* file_name = "output.txt", const char* decoded_name = "decoded.txt")
{
    unsigned long long *alphabet = new unsigned long long[256];
    for (int i = 0; i < 256; i++)
        alphabet[i] = 0;
    FILE *input = fopen(file_name, "rb");
    if (input == nullptr)
        throw invalid_argument("File not found.");
    char header = fgetc(input);
    int all_letters = (int)header;//Смотрим сколько символов используется в тексте
    char letter;
    for(int i=0; i < all_letters; i++)//После чего заполняем алфавит
    {
        letter = fgetc(input);
        fread(reinterpret_cast<char*>(&alphabet[letter]), sizeof(unsigned long long), 1, input);
    }
    priority_queue<knot, vector<knot>, knot> leafs;//Создание и заполнение очереди с помощью которой будет формироваться дерево
    cout << "------Alphabet------" << endl;
    for (int i = 0; i < 256; i++)
        if (alphabet[i] != 0)
        {
            knot new_leaf((char)i, alphabet[i]);
            cout << (char)i << ":" << alphabet[i] << " : " << new_leaf << endl;
            leafs.push(new_leaf);
        }
    cout << "------Tree------" << endl;
    while (leafs.size() > 1)//Само формирование дерева: последовательно берутся два элемента сверху и объединяются
    {                       //После чего их "предок" опять заносится в очередь
        knot* new_knot = new knot(leafs.top());
        leafs.pop();
        cout<< "Building: " << *new_knot << " + " << leafs.top() << endl;
        leafs.push(*new_knot->join(*new knot(leafs.top())));
        leafs.pop();
    }
    knot* tree = new knot(leafs.top());//Указатель на получившееся дерево
    FILE* output = fopen(decoded_name, "w +");
    tree->decode(alphabet, input, output);//Функция декодирования
    cout << "You did well!";
    fclose(input);//Проверка на правильность декодирования
    fclose(output);
    FILE*  final = fopen("decoded.txt", "r");
    FILE* initial = fopen("input.txt", "r");
    char after = 0, before = 0;
    while (!feof(initial) && !feof(final))
    {
        after = fgetc(final);
        before = fgetc(initial);
        if(!feof(initial) && !feof(final))
            if(after != before)
            {
                cout << "Not OK!";
                return;
            }
    }
    fclose(final);
    fclose(initial);
}

int main()
{
    decoder();
}