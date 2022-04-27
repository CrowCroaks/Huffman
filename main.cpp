#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <queue>

using namespace std;

class knot
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

    bool operator () (const knot& first, const knot& second)
    {
        return first.k >= second.k;
    }

    knot* join (knot new_elem)
    {
        return new knot( '0' , new_elem.k + k, new knot(new_elem), this);
    }

    void coding(knot* tree, string code, string* code_table)
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
    while (!feof(input))
    {
        letter = fgetc(input);
        if (!feof(input))
            alphabet[letter]++;
    }
    fclose(input);
    priority_queue<knot, vector<knot>, knot> leafs;
    cout << "------Alphabet------" << endl;
    for (int i = 0; i < 256; i++)
        if (alphabet[i] != 0)
        {
            knot new_leaf((char)i, alphabet[i]);
            cout << (char)i << ":" << alphabet[i] << " : " << new_leaf << endl;
            leafs.push(new_leaf);
        }
    cout << "------Tree------" << endl;
    while (leafs.size() > 1)
    {
        knot* new_knot = new knot(leafs.top());
        leafs.pop();
        cout<< "Building: " << *new_knot << " + " << leafs.top() << endl;
        leafs.push(*new_knot->join(*new knot(leafs.top())));
        leafs.pop();
    }
    knot* tree = new knot(leafs.top());
    string* code_table = new string [256];
    tree->coding(tree, "", code_table);
    cout << "------Huffman Codes------" << endl;
    for (int i = 0; i < 256; i++)
        cout << (char)i << ":" << code_table[i] << endl;
    FILE* output = fopen(encoded_name, "w +");
    input = fopen(file_name, "r");
    letter = 0;
}

int main()
{
    coder();
}