#include <iostream>
#include <stack>
#include <map>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <cassert>

std::map<std::string, int> operators = {{"(", 0}, {")", 1}, {"+", 2}, {"-", 2}, 
    {"*", 3}, {"/", 3}, {"^", 4}};

struct Node {
    std::string value;
    Node *left, *right, *parent;
    Node(std::string value, Node* left = nullptr, Node* right = nullptr, 
        Node* parent = nullptr) : 
            value(value), left(left), right(right), parent(parent) {}
};

class BinaryOperationTree {
private:
    Node* root;

    std::vector<std::string> parse_operation(std::string& str) {
        std::vector<std::string> res;
        std::string number;
        bool was_digit = false;
        bool was_operator = true;
        char last_operator;
        for (size_t i = 0; i < str.size(); i++) {
            if (isdigit(str[i])) {
                number += str[i];
                was_digit = true;
                was_operator = false;
            }
            else if (operators.count(std::string(1, str[i]))){
                if (was_digit) {
                    res.push_back(number); 
                    was_digit = false;
                    number = "";
                }
                else if (was_operator && last_operator != ')') {
                    if (str[i] == '-' || str[i] == '+') {
                        res.push_back("0");
                    }
                }
                res.push_back(std::string(1, str[i]));
                was_operator = true;
                last_operator = str[i];
            }
        }
        if (was_digit) {
            res.push_back(number);
        }
        return res;
    }

    std::vector<std::string> infix_to_postfix(
                                        const std::vector<std::string>& infix) 
    {
        std::vector<std::string> res;
        std::stack<std::string> stack_operators;
        for (size_t i = 0; i < infix.size(); i++) {
            if (isdigit(infix[i][0])) {
                res.push_back(infix[i]);
            }
            else if (infix[i] == "(") {
                stack_operators.push(infix[i]);
            }
            else if (infix[i] == ")") {
                while (!stack_operators.empty() &&
                        stack_operators.top() != "(")
                {
                    res.push_back(stack_operators.top());
                    stack_operators.pop();
                }
                stack_operators.pop();
            }
            else if (operators.count(infix[i])) {
                while (!stack_operators.empty() &&
                    operators[infix[i]] <= operators[stack_operators.top()])
                {
                    res.push_back(stack_operators.top());
                    stack_operators.pop();
                }
                stack_operators.push(infix[i]);
            }
        }
        while (!stack_operators.empty()) {
            res.push_back(stack_operators.top());
            stack_operators.pop();
        }
        return res;
    }

    Node* most_left(Node* from) {
        while (from->left) {
            from = from->left;
        }
        return from;
    }

    void remove_node(Node* node) {
        if (!node) {
            return;
        }
        remove_node(node->left); 
        remove_node(node->right);
        delete node;
    }

    double calc_node(Node* node) {
        if (operators.count(node->value)) {
            double left = calc_node(node->left);
            double right = calc_node(node->right);
            std::string oper = node->value;
            if (oper == "+") {
                return left + right;
            }
            else if (oper == "-") {
                return left - right;
            }
            else if (oper == "*") {
                return left * right;
            }
            else if (oper == "/") {
                return left / right;
            }
            else if (oper == "^") {
                return pow(left, right);
            }
        }
        return (std::stoi(node->value));
    }

    void print_node(Node* node, bool postfix, std::ostream& os) {
        if (!postfix) {
            if (node->left) {
                os << "(";
                print_node(node->left, postfix, os);
            }   
            os << node->value;
            if (node->right) {
                print_node(node->right, postfix, os);
                os << ")";
            }   
        }
        else {
            if (node->left) {
                print_node(node->left, postfix, os);
            }   
            if (node->right) {
                print_node(node->right, postfix, os);
            }   
            os << node->value << " ";
        }
    }

public:
    class Iterator;

    BinaryOperationTree() {
        root = nullptr;
    }

    ~BinaryOperationTree() {
        this->clear(); 
    }

    BinaryOperationTree(std::string& str)
    {
        std::vector<std::string> infix_expr = parse_operation(str);
        std::vector<std::string> postfix_expr = infix_to_postfix(infix_expr);
        std::stack<Node*> stack_nodes;
        for (size_t i = 0; i < postfix_expr.size(); i++) {
            if (operators.count(postfix_expr[i])) {
                Node* right = stack_nodes.top();
                stack_nodes.pop();
                Node* left = stack_nodes.top();
                stack_nodes.top() = new Node(postfix_expr[i], left, right);
                left->parent = stack_nodes.top();
                right->parent = stack_nodes.top();
            }
            else {
                Node* new_node = new Node(postfix_expr[i]);
                stack_nodes.push(new_node);
            }
        }
        root = stack_nodes.top();
    }

    BinaryOperationTree& operator=(std::string str) {
        clear();
        std::vector<std::string> infix_expr = parse_operation(str);
        std::vector<std::string> postfix_expr = infix_to_postfix(infix_expr);
        std::stack<Node*> stack_nodes;
        for (size_t i = 0; i < postfix_expr.size(); i++) {
            if (operators.count(postfix_expr[i])) {
                Node* right = stack_nodes.top();
                stack_nodes.pop();
                Node* left = stack_nodes.top();
                stack_nodes.top() = new Node(postfix_expr[i], left, right);
                left->parent = stack_nodes.top();
                right->parent = stack_nodes.top();
            }
            else {
                Node* new_node = new Node(postfix_expr[i]);
                stack_nodes.push(new_node);
            }
        }
        root = stack_nodes.top();
        return *this;
    }

    void clear() {
        remove_node(root);  
        root = nullptr;
    }

    double calc(Iterator iter = Iterator(nullptr)) {
        assert(root != nullptr);
        if (*iter == nullptr) {
            return calc_node(root); 
        }
        return calc_node(*iter);
    }

    void print(std::ostream& os = std::cout, bool postfix = false,
        Iterator iter = Iterator(nullptr))
    {
        assert(root != nullptr);
        Node* node = root;
        if (*iter != nullptr) {
            node = *iter;   
        }
        print_node(node, postfix, os);
    }

    class Iterator {
        private:
            Node* ptr;
            Node* most_left(Node* node) {
                while (node->left) {
                    node = node->left;
                }
                return node;
            }
        public:
            Iterator(Node* ptr = nullptr) : ptr(ptr) {}
            Iterator& operator++() {
                assert(ptr != nullptr);
                Node* tmp = ptr;
                if (!tmp->parent) {
                    ptr = nullptr;
                }
                else if (tmp->parent && tmp->parent->left == tmp) {
                    tmp = tmp->parent;
                    if (tmp->right) {
                        tmp = most_left(tmp->right);
                        ptr = tmp;
                    }
                    ptr = tmp;
                }
                else {
                    ptr = tmp->parent;
                }
                return *this;
            }

            Iterator operator++(int) {
                Iterator iter = *this;
                ++(*this);
                return iter;
            }
        
            Node* operator*() {
                return ptr;
            }

            bool operator==(const Iterator& iter) {
                return (ptr == iter.ptr);
            }

            bool operator!=(const Iterator& iter) {
                return (ptr != iter.ptr);
            }

    };

    Iterator begin() {
        Node* tmp = root;
        if (root->left) {
            tmp = most_left(root);
        }
        else if (root->right) {
            tmp = most_left(root->right);
        }
        return Iterator(tmp);
    }

    Iterator end() {
        return Iterator();
    }
};

std::ostream& operator<<(std::ostream& os, BinaryOperationTree& tree) {
    tree.print(os);
    return os;
}

std::istream& operator>>(std::istream& is, BinaryOperationTree& tree) {
    std::string str;
    getline(is, str);
    tree = str;
    return is;
}

int main() {
    std::string s;
    getline(std::cin, s);
    //s = infix_to_postfix(s);
    //cout << s << endl;
    BinaryOperationTree tree(s);
    std::cout << tree << std::endl;
    tree.print(std::cout, true);
    std::cout << std::endl;
    for (auto it = tree.begin(); it != tree.end(); it++) {
        tree.print(std::cout, false, it);
        std::cout << " = " << tree.calc(it) << std::endl;
    }
    return 0;
    /*
    tree = "(1 + 2 + 3 + 4) * 5";
    std::cout << tree << std::endl;
    std::cin >> tree;
    std::cout << tree << std::endl;
    tree.clear();
    std::cout << tree << std::endl;
    */
    std::cout << tree.calc() << std::endl;
    auto it = tree.begin();
    tree.print(std::cout, false, it);
    std::cout << std::endl;
    tree.print(std::cout, true, it);
    std::cout << std::endl;
    std::cout << tree.calc(it) << std::endl;
    ++it;
    tree.print(std::cout, false, it);
    std::cout << std::endl;
    tree.print(std::cout, true, it);
    std::cout << std::endl;
    std::cout << tree.calc(it) << std::endl;
    ++it;
    tree.print(std::cout, false, it);
    std::cout << std::endl;
    tree.print(std::cout, true, it);
    std::cout << std::endl;
    std::cout << tree.calc(it) << std::endl;
    return 0;
}
