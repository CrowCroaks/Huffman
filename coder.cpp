#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <queue>
#include <sys/stat.h>

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

    friend ostream& operator << (ostream& st, const knot& num)
    {
        st << num.key << ":" << num.k;
        return st;
    }

};

void coder(const char* file_name = "input.txt", const char* encoded_name = "output.txt")
{
    unsigned long long *alphabet = new unsigned long long[256];
    for (int i = 0; i < 256; i++)
        alphabet[i] = 0;
    FILE *input = fopen(file_name, "r");
    if (input == nullptr)
        throw invalid_argument("File not found.");
    char letter;
    while (!feof(input))//Формирование начального алфавита, составляющего текст
    {
        letter = fgetc(input);
        if (!feof(input))
            alphabet[letter]++;
    }
    fclose(input);
    priority_queue<knot, vector<knot>, knot> leafs;//Создание и заполнение очереди с помощью которой будет формироваться дерево
    cout << "------Alphabet------" << endl;
    for (int i = 0; i < 256; i++)
        if (alphabet[i] != 0)
        {
            knot new_leaf((char)i, alphabet[i]);
            cout << (char)i << ":" << alphabet[i] << " : " << new_leaf << endl;
            leafs.push(new_leaf);
        }
    char all_letters = leafs.size();//Запоминаем количество используемых симолов, необходимо для шапки
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
    string* code_table = new string [256];//Таблица хранящая коды Хаффмана
    tree->coding(tree, "", code_table);
    cout << "------Huffman Codes------" << endl;
    for (int i = 0; i < 256; i++)
        cout << (char)i << ":" << code_table[i] << endl;
    FILE* output = fopen(encoded_name, "wb");//Непосредственно начало кодирования файла
    input = fopen(file_name, "r");
    fputc(all_letters, output);
    for(int i=0; i<256; i++)//Формируем шапку, которая понадобится при декодировании: записываем символ + количество его вхождений(в виде массива символов)
        if(alphabet[i] != 0)
        {
            fputc((char)i, output);
            fwrite(reinterpret_cast<const char*>(&alphabet[i]), sizeof(unsigned long long), 1, output);
        }
    unsigned char symbol = 0, chunk = 0;
    int bits = 0, chunk_len = 0;
    while (!feof(input))
    {
        letter = fgetc(input);
        if(!feof(input))
        {
            if(bits + code_table[letter].length() <= 8)//Если символ влезает в байт полностью то записываем его
            {
                for(int i = 0; i < code_table[letter].length(); i++)
                    symbol = symbol << 1 | (code_table[letter][i] - '0');
                bits += code_table[letter].length();
            }
            else//Если символ влезает в байт не полностью
            {
                for(int i = 0; i < 8 - bits; i++)//Записываем то что записывается
                    symbol = symbol << 1 | (code_table[letter][i] - '0');
                if(code_table[letter].length() - 8 + bits >= 8)//Если под символ нужно выделить более одного нового байта
                {
                    int i = 8 - bits;
                    while(i + 7 < code_table[letter].length())//Начиная с бита на котором остановилась запись
                    {                                         //Пока символ может полностью заполнить байт
                        chunk = 0;
                        for(int j = 0; j < 8; j++)//Записываем его в новую переменную
                            chunk = chunk << 1 | (code_table[letter][i+j] - '0');
                        i += 8;
                        fputc(symbol, output);//После чего уже заполненный байт записываем и меняем его значение на только что сформированный
                        symbol = chunk;
                    }
                    chunk = 0;
                    chunk_len = 0;
                    for(int j = i; j < code_table[letter].length(); j++)//Записываем остаток
                    {
                        chunk = chunk << 1 | (code_table[letter][j] - '0');
                        chunk_len++;
                    }
                }
                else//Если нам достаточно одного нового байта
                {
                    chunk_len = 0;
                    for(int i = 8 - bits; i < code_table[letter].length(); i++)//Начиная с бита на котором остановилась запись
                    {
                        chunk = chunk << 1 | (code_table[letter][i] - '0');//Записываем символ в новую переменную
                        chunk_len++;
                    }
                }
                bits = 8;
            }
            if(bits == 8)//Если байт заполнен, записываем его
            {
                fputc(symbol, output);
                symbol = chunk;//+ если у нас после предыдущих действий остались незаписанные биты переходим к ним
                bits = chunk_len;
                chunk = 0;
                chunk_len = 0;
            }
        }
        else if(bits < 8)//Если последняя буква текста заполнила байт не полностью, "сдвигаем" ее в "начало" байта
        {
            symbol = symbol << (8 - bits);
            fputc(symbol, output);
        }
    }
    cout << "You did well!" << endl;
    fclose(input);
    fclose(output);
    long double file_full_size = 0, compress_size = 0;//Проверяем коэффициент сжатия
    struct stat sb{};
    struct stat se{};
    if (!stat(file_name, &sb))
        file_full_size = sb.st_size;
    else
        perror("stat");
    if (!stat(encoded_name, &se))
        compress_size = se.st_size;
    else
        perror("stat");
    cout << compress_size / file_full_size;
}

int main()
{
    coder();
}
