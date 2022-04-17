#include <iostream>
#include <cstdio>
#include <stdexcept>
#include <string>

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
        if (left && right)
        {
            delete[]left;
            delete[]right;
        }
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

    void hoarrec(int l, int r)
    {
        knot* array = this;
        if (l < r)
        {
            int i = l, j = r;
            unsigned long long x = array[(l + r) / 2].k;
            knot intermediate;
            while (i <= j)
            {
                while (array[i].k > x)
                    i++;
                while (array[j].k < x)
                    j--;
                if (i <= j)
                {
                    intermediate = array[i];
                    array[i] = array[j];
                    array[j] = intermediate;
                    i++;
                    j--;
                }
            }
            hoarrec(l, j);
            hoarrec(i, r);
        }
    }

    knot merge_knots (knot& descendant)
    {
        knot ancestor;
        ancestor.k = k + descendant.k;
        ancestor.left = this;
        ancestor.right = &descendant;
        return ancestor;
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
        st << num.key << ":" << num.k << endl;
        return st;
    }

};

void Hoar_sort(knot* array, int length)
{
    array->hoarrec(0, length - 1);
}

void coder(const char* file_name = "input.txt")
{
    unsigned long long* alphabet = new unsigned long long [256];
    int needleafs = 0;
    for (int i = 0; i < 256; i++)
        alphabet[i] = 0;
    FILE* input = fopen(file_name, "r");
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
    knot* leafs = new knot [256];
    for (int i = 0; i < 256; i++)
        if (alphabet[i] != 0)
        {
            knot new_leaf((char)i, alphabet[i]);
            leafs[needleafs] = new_leaf;
            needleafs++;
        }
    while (needleafs > 1)
    {
       Hoar_sort(leafs, needleafs);
       knot merger = leafs[needleafs - 1].merge_knots(leafs[needleafs - 2]);
       leafs[needleafs - 2] = merger;
       needleafs--;
    }
    knot tree = leafs[0];
    delete[]leafs;
    string* code_table = new string [256];
    tree.coding(&tree, "", code_table);
}

int main()
{
    coder();
}