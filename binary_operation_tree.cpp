#include <iostream>
#include <stack>
#include <map>
#include <vector>
#include <cstdlib>
#include <cmath>

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

    std::vector<std::string> parse_Operation(std::string& str) {
        std::vector<std::string> res;
        std::string number;
        bool was_digit = false;
        for (size_t i = 0; i < str.size(); i++) {
            if (isdigit(str[i])) {
                number += str[i];
                was_digit = true;
            }
            else if (operators.count(std::string(1, str[i]))){
                if (was_digit) {
                    res.push_back(number); 
                    was_digit = false;
                    number = "";
                }
                res.push_back(std::string(1, str[i]));
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
        std::vector<std::string> infix_expr = parse_Operation(str);
        std::vector<std::string> postfix_expr = infix_to_postfix(infix_expr);
        for (size_t i = 0; i < postfix_expr.size(); i++) {
            std::cout << postfix_expr[i] << " ";
        }
        std::cout << std::endl;
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
        std::vector<std::string> infix_expr = parse_Operation(str);
        std::vector<std::string> postfix_expr = infix_to_postfix(infix_expr);
        for (size_t i = 0; i < postfix_expr.size(); i++) {
            std::cout << postfix_expr[i] << " ";
        }
        std::cout << std::endl;
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
        if (*iter == nullptr) {
            return calc_node(root); 
        }
        return calc_node(*iter);
    }

    void print(Iterator iter = Iterator(nullptr), bool postfix = false,
            std::ostream& os = std::cout) 
    {
        Node* node = root;
        if (*iter != nullptr) {
            node = *iter;   
        }
        print_node(node, postfix, os);
    }

    void print_infix(std::ostream& os = std::cout) {
        if (!root) {
            return;
        }
        Node* tmp = root;
        if (!tmp->left) {
            os << root->value;
        }
        while(tmp->left) {
            os << "(";
            tmp = tmp->left;
        }
        while (tmp->parent) {
            if (tmp->right) {
                os << tmp->value;
                tmp = most_left(tmp->right);
            }
            else if (tmp->parent->left == tmp) {
                os << tmp->value;
                tmp = tmp->parent;
                if (tmp == root && tmp->right) {
                    os << root->value;
                    tmp = root->right;
                    while (tmp->left) {
                        os << "(";
                        tmp = tmp->left;
                    }
                }
            }
            else if (tmp->parent->right == tmp) {
                os << tmp->value;
                while (tmp->parent && tmp->parent->right == tmp) {
                    tmp = tmp->parent;
                    os << ")";
                }
                if (tmp->parent) {
                    tmp = tmp->parent;
                    if (tmp == root && tmp->right) {
                        os << root->value;
                        tmp = tmp->right;
                        while (tmp->left) {
                            os << "(";
                            tmp = tmp->left;
                        }
                    }
                }
            }
        }
    }

    class Iterator {
        private:
            Node* ptr;
            Node* most_left_operator(Node* node) {
                while (node->left && operators.count(node->left->value)) {
                    node = node->left;
                }
                return node;
            }
        public:
            Iterator(Node* ptr = nullptr) : ptr(ptr) {}
            Iterator& operator++() {
                Node* tmp = ptr;
                if (tmp->right && operators.count(tmp->right->value)) {
                    tmp = most_left_operator(tmp->right);
                }
                else {
                    while (tmp->parent && tmp->parent->right == tmp) {
                        tmp = tmp->parent;
                    }
                    if (!tmp->parent) {
                        ptr = nullptr;
                    }
                    else {
                        tmp = tmp->parent;
                        if (tmp->right && operators.count(tmp->right->value)) {
                            tmp = most_left_operator(tmp->right);
                        }
                        ptr = tmp;
                    }
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
    };

    Iterator begin() {
        Node* tmp = root;
        if (root->left && operators.count(root->left->value)) {
            tmp = most_left(root);
            tmp = tmp->parent;
        }
        else if (root->right && operators.count(root->right->value)) {
            tmp = most_left(root->right);
            tmp = tmp->parent;
        }
        return Iterator(tmp);
    }

    Iterator end() {
        return Iterator();
    }
};

std::ostream& operator<<(std::ostream& os, BinaryOperationTree& tree) {
    tree.print_infix(os);
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
    tree.print(it);
    std::cout << std::endl;
    tree.print(it, true);
    std::cout << std::endl;
    std::cout << tree.calc(it) << std::endl;
    ++it;
    tree.print(it);
    std::cout << std::endl;
    tree.print(it, true);
    std::cout << std::endl;
    std::cout << tree.calc(it) << std::endl;
    ++it;
    tree.print(it);
    std::cout << std::endl;
    tree.print(it, true);
    std::cout << std::endl;
    std::cout << tree.calc(it) << std::endl;
    return 0;
}
