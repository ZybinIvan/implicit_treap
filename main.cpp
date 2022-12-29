#include <iostream>
#include <algorithm>
#include <random>
#include <cstdlib>


std::mt19937 engine;

template<class TypeNode>
struct Node {
    size_t priority, size;
    TypeNode value, minValue, sum, maxValue;
    bool rev = false;
    Node *l = nullptr;
    Node *r = nullptr;

    explicit Node(TypeNode value) : priority(engine()), size(1), value(value), minValue(value), maxValue(value), sum(value) {}
};

template<class TypeTreap>
class Treap {
    Node<TypeTreap> *root = nullptr;

    // обновить поля узла
    static void update(Node<TypeTreap> *&n) {
        if (n) {
            n->size = getSize(n->l) + getSize(n->r) + 1;
            n->minValue = std::min(std::min(getMinValue(n->l), getMinValue(n->r)), n->value);
            n->sum = getSum(n->l) + getSum(n->r) + n->value;
            n->maxValue = std::max(std::max(getMaxValue(n->l), getMaxValue(n->r)), n->value);
        }
    }

    // объединить два дерева
    static Node<TypeTreap> *merge(Node<TypeTreap> *T1, Node<TypeTreap> *T2) {
        push(T1);
        push(T2);

        if (!T1 || !T2) {
            return T1 ? T1 : T2;
        } else if (T1->priority > T2->priority) {
            T1->r = merge(T1->r, T2);
            update(T1); // обновляем данные о размере поддерева
            return T1;
        } else {
            T2->l = merge(T1, T2->l);
            update(T2); // обновляем данные о размере поддерева
            return T2;
        }
    }

    // разделить дерево по index
    static void split(Node<TypeTreap> *n, size_t index, Node<TypeTreap> *&T1, Node<TypeTreap> *&T2) {
        push(n);
        if (!n) {
            T1 = T2 = nullptr;
            return;
        } else if (getSize(n->l) < index) {
            split(n->r, index - getSize(n->l) - 1, n->r, T2);
            T1 = n;
        } else {
            split(n->l, index, T1, n->l);
            T2 = n;
        }
        update(T1); // обновляем размеры новых корней
        update(T2);
    }

    // Очищение дерева/поддерева
    void deleteTree(Node<TypeTreap> *T) {
        if (T->l) {
            deleteTree(T->l);
        }
        if (T->r) {
            deleteTree(T->r);
        }
        delete T;
    }

public:
    // Деструктор
    ~Treap() {
        if (root) {
            deleteTree(root);
        }
    }

    static void push(Node<TypeTreap> *n) {
        if (!n || !n->rev) {
            return;
        }

        std::swap(n->l, n->r);

        if (n->l) {
            n->l->rev ^= true;
        }
        if (n->r) {
            n->r->rev ^= true;
        }
        n->rev = false;
    }

    void reverse(size_t l, size_t r) {
        Node<TypeTreap> *less, *equal, *greater;
        split(root, l, less, greater);
        split(greater, r - l + 1, equal, greater);
        equal->rev ^= 1;
        root = merge(merge(less, equal), greater);
    }

    static TypeTreap getMinValue(Node<TypeTreap> *n) {
        return n ? n->minValue : 2e9;
    }

    TypeTreap minValue(size_t l, size_t r) {
        Node<TypeTreap> *less, *greater, *equal;
        split(root, l, less, greater);
        split(greater, r - l + 1, equal, greater);
        TypeTreap result = getMinValue(equal);
        root = merge(merge(less, equal), greater);
        return result;
    }

    // Получить размера дерева/поддерева
    static size_t getSize(Node<TypeTreap> *n) {
        return n ? n->size : 0;
    }


    TypeTreap getValue(size_t index) {
        Node<TypeTreap> *less, *equal, *greater;
        split(root, index, less, greater); // нужный нам элемент попадает в greater, где находится под индексом "0"
        split(greater, 1, equal, greater); // отделяем нужный нам элемент от greater
        TypeTreap result = equal->value;
        root = merge(merge(less, equal), greater); // возвращаем дерево в исходное положение
        return result;
    }

    Node<TypeTreap> *getNode(size_t index) {
        Node<TypeTreap> *less, *equal, *greater;
        split(root, index, less, greater); // нужный нам элемент попадает в greater, где находится под индексом "0"
        split(greater, 1, equal, greater); // отделяем нужный нам элемент от greater
        root = merge(merge(less, equal), greater); // возвращаем дерево в исходное положение
        return equal;
    }

    size_t size() {
        return getSize(root);
    }

    void insert(size_t index, TypeTreap value) {
        Node<TypeTreap> *less, *greater;
        split(root, index, less, greater); // делим наше дерево по индексу, куда проводится вставка
        less = merge(less, new Node<TypeTreap>(value)); // склеиваем одно из поддеревьев с новым узлом
        root = merge(less, greater); // возвращаем дерево в исходное положение
    }

    void pushBack(TypeTreap value) {
        root = merge(root, new Node<TypeTreap>(value));
    }

    void pushFront(TypeTreap value) {
        root = merge(new Node<TypeTreap>(value), root);
    }

    void erase(size_t index) {
        Node<TypeTreap> *less, *greater, *equal;
        split(root, index, less, greater); // удаляемый элемент попадает в greater
        split(greater, 1, equal, greater); // удаляемый находится под индексом "0", поэтому делим по "1"
        root = merge(less, greater); // строим дерево без удаленного элемента
        free(equal); // освобождаем память
    }

    void erase(size_t l, size_t r) {
        if (l > r) { // вызов функции от (1, 2) или (2, 1) будет расцениваться одинаково
            erase(r, l);
        }
        Node<TypeTreap> *less, *greater, *equal;
        split(root, l, less, greater);
        split(greater, r - l + 1, equal, greater);
        root = merge(less, greater); // объединение участков без удаляемых элементов
        deleteTree(equal); // очистка удаляемых элементов
    }

    void replaceToPos(size_t l, size_t r, size_t pos) {
        Node<TypeTreap> *less, *greater, *equal, *moveToLeft, *moveToRight;
        split(root, l, less, greater);
        split(greater, r - l + 1, equal, greater);
        if (pos > l) {
            split(greater, pos - l, moveToLeft, greater);
            less = merge(less, moveToLeft);
        } else if (pos < l) {
            split(less, less->size - l + pos, less, moveToRight);
            greater = merge(moveToRight, greater);
        }
        merge(merge(less, equal), greater);
    }

    void toFront(size_t l, size_t r) {
        Node<TypeTreap> *less, *greater, *equal;
        split(root, l, less, greater);
        split(greater, r - l + 1, equal, greater); // разделили на три поддерева
        root = merge(merge(equal, less), greater); // склеиваем так, чтобы поддерево equal было в начале
    }

    static TypeTreap getSum(Node<TypeTreap> *n) {
        return n ? n->sum : 0;
    }

    TypeTreap getSum(size_t l, size_t r) {
        Node<TypeTreap> *less, *equal, *greater;
        split(root, l, less, greater);
        split(greater, r - l + 1, equal, greater); // отделили промежуток
        TypeTreap result = getSum(equal); // получили суммы
        root = merge(merge(less, equal), greater); // собрали дерево
        return result;
    }

    static TypeTreap getMaxValue(Node<TypeTreap> *n) {
        return n ? n->maxValue : -2e9;
    }

    TypeTreap maxValue(size_t l, size_t r) {
        Node<TypeTreap> *less, *greater, *equal;
        split(root, l, less, greater);
        split(greater, r - l + 1, equal, greater);
        TypeTreap result = getMaxValue(equal);
        root = merge(merge(less, equal), greater);
        return result;
    }

    void print() {
        for (size_t i = 0; i < root->size; ++i) {
            std::cout << "Value: " << getValue(i) << " Priority: " << getP(getNode(i)) << " \n";
        }
    }
};

int main() {
    Treap<int> T;

    int n, q;
    std::cin >> n >> q;

    for (int i = 0; i < n; ++i) {
        int d;
        std::cin >> d;
        T.pushBack(d);
    }

    for (int j = 0; j < q; ++j) {
        char s;
        std::cin >> s;
        if (s == '=') {
            int i, d;
            std::cin >> i >> d;
            T.erase(i - 1);
            T.insert(i - 1, d);
        } else if(s == '?') {
            int f, t;
            std::cin >> f >> t;
//            T.print();
            std::cout << T.getSum(f - 1, t - 1) << "\n";
        }
    }


    return 0;
}