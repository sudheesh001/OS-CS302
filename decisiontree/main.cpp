#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#include <time.h>

using namespace std;

struct Node
{
    Node(int v) {value = v;}
    ~Node();

    map<string, Node*> children;
    int value;
};

struct Object
{
    string classification;
    string attribute[7];
};

string majorityValue(vector<Object> examples)
{
    map<string, int> popularity;
    for (uint8_t i=0; i<examples.size(); i++) {
        popularity[examples[i].classification]++;
    }

    string mp=examples[0].classification;
    int p=popularity[mp];

    for (uint8_t i=1; i<examples.size(); i++) {
        if (popularity[examples[i].classification] > p) {
            mp = examples[i].classification;
            p=popularity[examples[i].classification];
        }
    }
    return mp;
}

map<string, int> splitByAttribute(vector<Object> examples, int attribute)
{
    map<string, int> results;
    for (uint8_t i=0; i<examples.size(); i++) {
        results[examples[i].attribute[attribute]]++;
    }
    return results;
}

double calculateGain(vector<Object> examples, int attribute)
{
    map<string, int> sizes = splitByAttribute(examples, attribute);
    double totalSize = examples.size();
    double remainder = 0.0;
    map<string, int>::iterator it;
    for (it=sizes.begin(); it!=sizes.end(); it++) {
        double reducedSize = (*it).second;
        remainder += (reducedSize / totalSize);
    }
    return remainder;
}

int chooseAttributeInfoGain(vector<Object> examples, vector<int> attributes)
{
    double bestGain = 0.0;
    int bestAttribute = attributes[0];
    for (uint8_t i = 0; i < attributes.size(); i++) {
        double gain = calculateGain(examples, attributes[i]);
        if (gain > bestGain) {
            bestGain = gain;
            bestAttribute = attributes[i];
        }
    }
    return bestAttribute;
}

int chooseAttributeRandom(vector<Object> examples, vector<int> attributes)
{
    int att = rand() % attributes.size();
    return attributes[att];
}

Node *DTL(vector<Object> examples, vector<int> attributes, Node *def, int (*chooseAttributeFunction)(vector<Object>, vector<int>) )
{
    if (examples.size() == 0) return def;

    {
        string classification = examples[0].classification;
        bool allSame = true;
        for (uint8_t i=0; i<examples.size(); i++) {
            if (examples[i].classification != classification) {
                allSame = false;
                break;
            }
        }
        if (allSame)
            return new Node(atoi(classification.c_str()));
    }

    if (attributes.size() == 0) return new Node(atoi(majorityValue(examples).c_str()));

    int best = chooseAttributeFunction(examples, attributes);
    {
        vector<int>::iterator it;
        for (it = attributes.begin(); it != attributes.end(); it++) {
            if ((*it) == best) {
                attributes.erase(it);
                break;
            }
        }
    }
//    cout << best << endl;

    Node *root = new Node(best);
    
    string m = majorityValue(examples);

    vector<string> choices;
    choices.push_back(string("1"));
    choices.push_back(string("2"));
    for(int i=0; i<2; i++) {
        // Add all relevant examples
        vector<Object> newExamples;
        for (uint8_t j=0; j<examples.size(); j++) {
            if (examples[j].attribute[best] == choices[i]) {
                newExamples.push_back(examples[j]);
            }
        }

        Node *subtree = DTL(newExamples, attributes, new Node(atoi(m.c_str())), chooseAttributeFunction);
        root->children.insert(pair<string, Node*>(choices[i], subtree));
    }
    return root;
}

void printTree(Node *root, int offset=0) {
    for (int i=0; i<offset; i++) cout << "\t";
    cout << "VAL: " << root->value << endl;
    map<string, Node*>::iterator it;
    for (it=root->children.begin(); it != root->children.end(); it++) {
        printTree((*it).second, offset++);
    }
}

vector<Object> readObjects(char *filename)
{
    ifstream file(filename);
    vector<Object> objects;
    while (!file.eof()) {
        string line;
        getline(file, line);
        istringstream iss(line);

        Object object;
        for (int i=0; i<7; i++) {
            string token;
            iss >> token;
            object.attribute[i] = token;
        }
        {
            string token;
            iss >> token;
            object.classification = token;
        }
        objects.push_back(object);
    }
    return objects;
}

int classify(Object object, Node *tree, int depth=0)
{
    if (tree->children.size() == 0) return tree->value;
    
    return classify(object, tree->children[object.attribute[depth]], depth+1);
}

double testAccuracy(Node *tree, vector<Object> testData)
{
    double sum = 0.0;
    for (uint8_t i=0; i<testData.size(); i++) {
        if (classify(testData[i], tree) == atoi(testData[i].classification.c_str())) sum += 1.0;
    }
    sum /= testData.size();
    sum *= 100.0;
    return sum;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        cout << "Usage: `" << argv[0] << " training.txt [test.txt]' where filename is a name containing training data, and test.txt is an optional file with test data." << endl;
        return 1;
    }
    srand(time(NULL));

    // Get example data
    vector<Object> examples;
    {
        ifstream file(argv[1]);
        if (!file) {
            cout << "Usage: `" << argv[0] << " training.txt [test.txt]' where filename is a name containing training data, and test.txt is an optional file with test data." << endl;
            return 1;
        }
        examples = readObjects(argv[1]);
    }

    // Generate initial list of attributes
    vector<int> attributes;
    for (int i=0; i<7; i++) {
        attributes.push_back(i);
    }

    Node *infoGainTree = DTL(examples, attributes, new Node(0), chooseAttributeInfoGain);
    Node *randomTree = DTL(examples, attributes, new Node(0), chooseAttributeRandom);

    if (argc == 3) {
        ifstream file (argv[2]);
        if (!file) {
            cout << "Usage: `" << argv[0] << " training.txt [test.txt]' where filename is a name containing training data, and test.txt is an optional file with test data." << endl;
            return 1;
        }
        vector<Object> testObjects = readObjects(argv[2]);
        cout << "Tree based on information gain based selection of attributes: " << testAccuracy(infoGainTree, testObjects) <<"\% accurate." << endl;
        cout << "Tree based on random selection of attributes: " << testAccuracy(randomTree, testObjects)<< "\% accurate" << endl;
    } else {
        cout << endl << "Infogain tree:" << endl;
        printTree(infoGainTree);
        cout << endl << "Random tree:" << endl;
        printTree(randomTree);
    }

    return 0;
}
