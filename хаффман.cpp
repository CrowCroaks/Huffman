#include <iostream>
#include <stdio.h>
#include <errno.h>
#include <queue>

using namespace std;

class knot
{
    char key;
    unsigned long long k;
    knot *left, *right;

public:
    
    knot(const char& letter = '0', unsigned long long amount = 0, knot* l = NULL, knot* r = NULL)
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
        left = NULL;
        right = NULL;
    }

    friend ostream& operator << (ostream& st, const knot& num)
    {
        st << num.k << endl;
        return st;
    }

    bool operator() (const knot& first, const knot& second)
    {
        return first.k >= second.k;
    }
};

void coder(const char* file_name = "input.txt")
{
    unsigned long long* alphabet = new unsigned long long [256];
    for (int i = 0; i < 256; i++)
        alphabet[i] = 0;
    FILE* input;
    errno_t in = fopen_s(&input, file_name, "r");
    if (in == 0)
    {
        char letter;
        while (!feof(input))
        {
            letter = fgetc(input);
            if (!feof(input))
                alphabet[letter]++;
        }
        fclose(input);
    }
    priority_queue<knot, vector<knot>, knot> leaf;
    for (int i = 0; i < 256; i++)
        if (alphabet[i] != 0) 
        {
            char s = (char)i;
            knot new_leaf(s, alphabet[i]);
            cout << s << " : " << alphabet[i] << endl;
            leaf.push(new_leaf);
        }
}

int main()
{
    coder();
}

