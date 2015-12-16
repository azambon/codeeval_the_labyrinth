/* 
 * File:   main.cpp
 * Author: Andrea Zambon
 *
 * Created on 29 novembre 2015, 11.18
 */

#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <fstream>
#include <string.h>

#include <stack>
#include <stdlib.h>
#include <stdint.h>
#include <vector>

//#define SANITY_CHECK


template <typename T>
class GraphNode;

template <typename T>
class GraphEdge {
public:

    GraphEdge(GraphNode<T>& first, GraphNode<T>& second, int weight) :
    first(first), second(second) {
        this->weight = weight;
    }

    virtual ~GraphEdge() {
        unsetEdgeFromNode(first);
        unsetEdgeFromNode(second);
    }

    GraphNode<T>& getFirst() const {
        return first;
    }

    GraphNode<T>& getSecond() const {
        return second;
    }

    int getWeight() const {
        return weight;
    }
private:

    //Non const per poter avere dei riferimenti modificabili ai nodi
    GraphNode<T>& first;
    GraphNode<T>& second;

    int weight;

    void unsetEdgeFromNode(GraphNode<T>& node) const {
        if (node.getN() == this) {
            node.setN(0);
        }

        if (node.getE() == this) {
            node.setE(0);
        }

        if (node.getS() == this) {
            node.setS(0);
        }

        if (node.getW() == this) {
            node.setW(0);
        }
    }

    GraphEdge(const GraphEdge& orig);
    GraphEdge operator=(const GraphEdge& otherEdge);
};

template <typename T>
class GraphNode {
public:

    GraphNode(int x, int y) {
        this->x = x;
        this->y = y;

        this->n = 0;
        this->e = 0;
        this->s = 0;
        this->w = 0;

        this->payload = 0;
    }

    GraphNode(int x, int y, T* payload) {
        this->x = x;
        this->y = y;

        this->n = 0;
        this->e = 0;
        this->s = 0;
        this->w = 0;

        this->payload = payload;
    }

    virtual ~GraphNode() {
        clearEdges();
    }

    int getX() const {
        return x;
    }

    int getY() const {
        return y;
    }

    const GraphEdge<T>* getN() const {
        return n;
    }

    const GraphEdge<T>* getE() const {
        return e;
    }

    const GraphEdge<T>* getS() const {
        return s;
    }

    const GraphEdge<T>* getW() const {
        return w;
    }

    void setN(GraphEdge<T>* edge) {
        n = edge;
    }

    void setE(GraphEdge<T>* edge) {
        e = edge;
    }

    void setS(GraphEdge<T>* edge) {
        s = edge;
    }

    void setW(GraphEdge<T>* edge) {
        w = edge;
    }

    T* getPayload() const {
        return payload;
    }

    void setPayload(T* newPayload) {
        payload = newPayload;
    }

    int countEdges() const {
        int res = 0;
        if (this->n != 0) {
            ++res;
        }
        if (this->e != 0) {
            ++res;
        }
        if (this->s != 0) {
            ++res;
        }
        if (this->w != 0) {
            ++res;
        }
        return res;
    }
private:
    //Coordinate del nodo
    int x;
    int y;

    T* payload;

    GraphEdge<T>* n;
    GraphEdge<T>* e;
    GraphEdge<T>* s;
    GraphEdge<T>* w;

    void clearEdges() {
        if (this->n != 0) {
            delete this->n;
        }

        if (this->e != 0) {
            delete this->e;
        }

        if (this->s != 0) {
            delete this->s;
        }

        if (this->w != 0) {
            delete this->w;
        }
    }
    //void removeEdge(GraphEdge<T>* edge) const;

    GraphNode(const GraphNode& orig);
    GraphNode operator=(const GraphNode& otherNode);
};

template <typename T>
class FibonacciHeapNode {
public:

    FibonacciHeapNode() {
        reset(0);
    }

    void reset(int value) {
        reset(value, 0, this, this);
    }

    void reset(int value, FibonacciHeapNode* parent) {
        reset(value, parent, this, this);
    }

    void reset(int value, FibonacciHeapNode* parent, FibonacciHeapNode* prevSibling, FibonacciHeapNode* nextSibling) {
        this->value = value;
        marked = false;
        rank = 0;
        this->parent = parent;
        this->prevSibling = prevSibling;
        this->nextSibling = nextSibling;
        firstChild = 0;
        payload = 0;
    }

    int getValue() const {
        return value;
    }

    void setValue(int newValue) {
        value = newValue;
    }

    int getRank() const {
        return rank;
    }

    bool isMarked() const {
        return marked;
    }

    void mark() {
        marked = true;
    }

    void unmark() {
        marked = false;
    }

    void addChild(FibonacciHeapNode* newChild) {
        if (newChild->parent != this) {
            if (firstChild != 0) {
                firstChild->addSiblingBefore(newChild);
            } else {
                //Il nodo da aggiungere potrebbe provenire da una lista di nodi. Quindi
                //resettiamo il suo prev e next
                newChild->prevSibling = newChild->nextSibling = newChild;
                firstChild = newChild;
            }
            newChild->parent = this;
            ++rank;
        }
    }

    void addSiblingBefore(FibonacciHeapNode* newSibling) {
        if (newSibling->nextSibling != this) {
            addSiblingBeforeNoCheck(newSibling);
        }
    }

    void addSiblingBeforeNoCheck(FibonacciHeapNode* newSibling) {
        prevSibling->nextSibling = newSibling;
        newSibling->prevSibling = prevSibling;
        newSibling->nextSibling = this;
        prevSibling = newSibling;
    }

    void addSiblingsBefore(FibonacciHeapNode* firstNewSibling) {
        FibonacciHeapNode* lastNewSibling = firstNewSibling->prevSibling;
        prevSibling->nextSibling = firstNewSibling;
        firstNewSibling->prevSibling = prevSibling;
        lastNewSibling->nextSibling = this;
        prevSibling = lastNewSibling;
    }

    //Non aggiorna il rango del padre e neanche il puntatore al primo figlio!
    void removeThisNodeFromSiblingsList() {
        //Dovrebbe funzionare anche nel caso di una lista con solo questo nodo
        prevSibling->nextSibling = nextSibling;
        nextSibling->prevSibling = prevSibling;
    }

    void moveChildrenToNewParent(FibonacciHeapNode* newParent) {
        if (firstChild != 0 && newParent != this) {
            FibonacciHeapNode<T>* child = firstChild;

            //Aggiorna il parent di tutti i figli
            do {
                child->parent = newParent;
                child = child->nextSibling;
            } while (child != firstChild);

            if (newParent != 0) {
                if (newParent->firstChild == 0) {
                    newParent->firstChild = firstChild;
                } else {
                    newParent->firstChild->addSiblingsBefore(firstChild);
                }
                newParent->rank += rank;
            }

            //Vuota questo nodo
            rank = 0;
            firstChild = 0;
        }
    }

    //Si limita ad aggiornare il parent dei figli
    void moveChildrenToRootList() {
        if (firstChild != 0) {
            FibonacciHeapNode<T>* child = firstChild;
            do {
                child->parent = 0;
                child = child->nextSibling;
            } while (child != firstChild);

            //Azzera questo nodo
            rank = 0;
            firstChild = 0;
        }
    }

    //Non aggiorna il numero delle radici e non aggiunge effettivamente il nodo alla
    //lista. Prepara solo il nodo ed aggiorna il vecchio padre
    void moveNodeToRootList() {
        if (parent != 0) {
            if (parent->rank == 1) {
                //Il padre aveva un solo figlio, quindi dopo questa operazione
                //non avrà più figli
                parent->firstChild = 0;
            } else {
                if (parent->firstChild == this) {
                    //Il primo figlio del padre è il nodo che stiamo togliendo. Passa
                    //ad usare il figlio successivo
                    parent->firstChild = nextSibling;
                }

                //Togli questo nodo dalla lista dei figli del suo padre
                removeThisNodeFromSiblingsList();
            }

            //Aggiorna il rango del padre
            --parent->rank;

            //Adesso questo nodo è una radice, quindi non ha più un padre
            parent = 0;
        }
    }

    FibonacciHeapNode<T>* getParent() const {
        return parent;
    }

    FibonacciHeapNode<T>* getFirstChild() const {
        return firstChild;
    }

    FibonacciHeapNode<T>* getPrevSibling() const {
        return prevSibling;
    }

    FibonacciHeapNode<T>* getNextSibling() const {
        return nextSibling;
    }

    T* getPayload() const {
        return payload;
    }

    void setPayload(T* newPayload) {
        payload = newPayload;
    }

#ifdef SANITY_CHECK

    int checkNodeSanity() const {
        int nodeCount = 1;

        if (rank > 0) {
            std::vector<FibonacciHeapNode<T>*> children;

            FibonacciHeapNode<T>* firstNode = firstChild;
            children.push_back(firstNode);

            if (firstNode->getValue() < value) {
                throw "Heap property violated";
            }
            if (firstNode->getParent() != this) {
                throw "Child node doesn't point to the correct parent";
            }

            FibonacciHeapNode<T>* currentNode = firstNode->getNextSibling();
            int childrenCount = 1;
            nodeCount += firstNode->checkNodeSanity();
            while (currentNode != firstNode && childrenCount < rank) {
                if (currentNode->getValue() < value) {
                    throw "Heap property violated";
                }
                if (currentNode->getParent() != this) {
                    throw "Child node doesn't point to the correct parent";
                }
                children.push_back(currentNode);
                nodeCount += currentNode->checkNodeSanity();
                ++childrenCount;

                currentNode = currentNode->getNextSibling();
            }

            if (currentNode != firstNode) {
                throw "Children list is longer than the recorded length";
            } else if (childrenCount < rank) {
                throw "Children list is shorter than the recorded length";
            }

            currentNode = firstNode->getPrevSibling();
            int index = rank - 1;
            childrenCount = 1;
            while (currentNode != firstNode && childrenCount < rank) {
                if (children.at(index--) != currentNode) {
                    throw "Different sequence when going backward";
                }
                ++childrenCount;
                currentNode = currentNode->getPrevSibling();
            }

            if (currentNode != firstNode) {
                throw "Children list is longer than the recorded length when going in reverse";
            } else if (childrenCount < rank) {
                throw "Children list is shorter than the recorded length when going in reverse";
            }
        }

        return nodeCount;
    }
#endif

    void clear() {
        if (firstChild) {
            FibonacciHeapNode<T>* endNode = firstChild;
            FibonacciHeapNode<T>* node = firstChild;
            FibonacciHeapNode<T>* nextNode;

            do {
                nextNode = node->nextSibling;
                delete node;
                node = nextNode;
            } while (node != endNode);
        }

        firstChild = 0;
        rank = 0;
        marked = 0;
    }

    virtual ~FibonacciHeapNode() {
        clear();
    }

private:
    int value;
    int rank; //Numero dei figli diretti
    bool marked;
    FibonacciHeapNode* parent;
    FibonacciHeapNode* firstChild;
    FibonacciHeapNode* prevSibling;
    FibonacciHeapNode* nextSibling;

    T* payload;

    void updateRank();

    FibonacciHeapNode(const FibonacciHeapNode& orig);
    FibonacciHeapNode& operator=(const FibonacciHeapNode& rhs);
};

template <typename T>
class PriorityQueue;

template<typename T>
class PriorityQueueItem {
    friend PriorityQueue<T>;
public:

    T* getPayload() const {
        return payload;
    }

    void setPayload(T* newPayload) {
        payload = newPayload;
    }
private:
    FibonacciHeapNode<PriorityQueueItem<T> >* heapNode;
    T* payload;

    PriorityQueueItem(T* payload, FibonacciHeapNode<PriorityQueueItem<T> >* node) {
        this->payload = payload;
        //Linka questo oggetto con il nodo dell'heap
        heapNode = node;
        node->setPayload(this);
    }

    PriorityQueueItem& operator=(const PriorityQueueItem<T>& rhs);
    PriorityQueueItem(const PriorityQueueItem<T>& orig);

    virtual ~PriorityQueueItem() {
        heapNode->setPayload(0);
    }
};

class Payload {
public:
    Payload(GraphNode<Payload>* node);
    virtual ~Payload();

    void setPriorityQueueItem(PriorityQueueItem<Payload>* item);
    PriorityQueueItem<Payload>* getPriorityQueueItem() const;

    GraphNode<Payload>* getNode() const;
    const GraphEdge<Payload>* getCameFromEdge() const;
    void setCamePromEdge(const GraphEdge<Payload>* cameFrom);

    int getDistance() const;
    void setDistance(int newDistance);

    int getTentativeDistance() const;
    void setTentativeDistance(int newDistance);

    bool isInClodesSet() const;
private:
    PriorityQueueItem<Payload>* pqi;
    GraphNode<Payload>* node;
    const GraphEdge<Payload>* came_from_edge;
    int distance;
    int tentative_distance;

    Payload(const Payload& orig);
};

Payload::Payload(GraphNode<Payload>* node) {
    this->node = node;
    pqi = 0;
    distance = -1;
    tentative_distance = 0;
    came_from_edge = 0;
}

void Payload::setPriorityQueueItem(PriorityQueueItem<Payload>* item) {
    pqi = item;
}

PriorityQueueItem<Payload>* Payload::getPriorityQueueItem() const {
    return pqi;
}

GraphNode<Payload>* Payload::getNode() const {
    return node;
}

int Payload::getDistance() const {
    return distance;
}

void Payload::setDistance(int newDistance) {
    distance = newDistance;
}

int Payload::getTentativeDistance() const {
    return tentative_distance;
}

void Payload::setTentativeDistance(int newDistance) {
    tentative_distance = newDistance;
}

const GraphEdge<Payload>* Payload::getCameFromEdge() const {
    return came_from_edge;
}

void Payload::setCamePromEdge(const GraphEdge<Payload>* cameFrom) {
    came_from_edge = cameFrom;
}

bool Payload::isInClodesSet() const {
    return distance >= 0;
}

Payload::~Payload() {
}

class Labyrinth {
public:
    Labyrinth(char* input, const int rows, const int columns);
    virtual ~Labyrinth();

    void drawSolution();
private:
    char* input;
    const int rows;
    const int columns;
    GraphNode<Payload>** nodes;
    GraphNode<Payload>* upperExitNode;
    GraphNode<Payload>* lowerExitNode;

    GraphNode<Payload>* goToNextIntersectionAndCreateNodeIfItDoesntExist(GraphNode<Payload>* node, int x, int y);
    int navigateToNextIntersection(GraphNode<Payload>* node, int &x, int &y, int &prevPositionIndex, void (Labyrinth::*cellCallback)(int, int) = 0);
    int heuristic(int x, int y) const;
    void processNodeAtTheOtherEndOfEdge(const GraphNode<Payload>* currentNode, const GraphEdge<Payload>* edge, PriorityQueue<Payload>* pq) const;
    GraphNode<Payload>* getNodeAtTheOtherEndOfEdge(const GraphNode<Payload>* currentNode, const GraphEdge<Payload>* edge) const;
    void outputSolution();
    void drawPath(GraphNode<Payload>* node, const GraphEdge<Payload>* edge);
    void drawX(int x, int y);
    GraphNode<Payload>* getStartNode() const;
    GraphNode<Payload>* getEndNode() const;

    Labyrinth(const Labyrinth& orig);
    Labyrinth operator=(const Labyrinth& otherNode);
};

template <typename T>
class FibonacciHeap {
public:

    FibonacciHeap() {
        nodeCount = 0;
        rootsCount = 0;
        minNode = 0;
    }

    virtual ~FibonacciHeap() {
    }

    bool empty() const {
        return nodeCount == 0;
    }

    int count() const {
        return nodeCount;
    }

    void clear() {
        if (minNode != 0) {
            FibonacciHeapNode<T>* endNode = minNode;
            FibonacciHeapNode<T>* node = minNode;
            FibonacciHeapNode<T>* nextNode;

            do {
                nextNode = node->getNextSibling();
                delete node;
                node = nextNode;
            } while (node != endNode);
        }

        nodeCount = 0;
        rootsCount = 0;
        minNode = 0;
    }

    FibonacciHeapNode<T>* insert(int value) {
        //Aggiungi una nuova radice
        ++nodeCount;
        ++rootsCount;
        FibonacciHeapNode<T>* newNode = getEmptyNode();
        newNode->reset(value);

        if (minNode == 0) {
            minNode = newNode;
        } else {
            minNode->addSiblingBeforeNoCheck(newNode);
            if (value < minNode->getValue()) {
                minNode = newNode;
            }
        }

#ifdef SANITY_CHECK
        checkHeapSanity();
#endif

        return newNode;
    }

    void deleteMin() {
        FibonacciHeapNode<T>* newMinNode;
        if (minNode->getRank() == 0) {
            if (rootsCount == 1) {
                //C'è un solo nodo nello heap (il minimo, che stiamo togliendo adesso)
                newMinNode = 0;
            } else {
                newMinNode = minNode->getNextSibling();
                minNode->removeThisNodeFromSiblingsList();
            }
            --rootsCount;
        } else {
            //Scegli arbitrariamente un nuovo nodo minimo. Può capitare che il nodo
            //scelto non sia il nodo minimo, ma questo non è un problema. In seguito
            //questo nodo scelto verrà corretto per puntare veramente al nodo minimo
            newMinNode = minNode->getFirstChild();

            int minNodeRank = minNode->getRank();

            //Aggiorna il parent dei nodi
            minNode->moveChildrenToRootList();

            if (rootsCount > 1) {
                //Aggiungi i nodi alla lista delle radici
                minNode->addSiblingsBefore(newMinNode);
                //Togli il vecchio nodo minimo dalla lista
                minNode->removeThisNodeFromSiblingsList();
            }

            //Togli uno per la radice che è stata eliminata e poi aggiungi il
            //numero di figli del nodo rimosso
            rootsCount += minNodeRank - 1;
        }

        //Cancella il vecchio nodo minimo e scegline uno nuovo
        --nodeCount;
        deleteNode(minNode);
        minNode = newMinNode;

        if (rootsCount > 0) {
            //Unisci i nodi con lo stesso rango e trova il vero minimo
            //Il massimo indice usato è nodeCount-1, quindi serve un array di
            //nodeCount elementi
            FibonacciHeapNode<T>** rankRegistry = getNodeArrayWithAtLeastNElements(nodeCount);

            FibonacciHeapNode<T>* currentNode = minNode;
            rankRegistry[currentNode->getRank()] = currentNode;
            currentNode = currentNode->getNextSibling();
            int currentNodeRank;
            FibonacciHeapNode<T>* otherNodeWithSameRank;
            FibonacciHeapNode<T>* insertBefore;

            //Si parte da 1 perchè un nodo è già stato considerato prima del loop.
            //rootsCount può venire modificato (ridotto) durante il loop!
            for (int i = 1; i < rootsCount; ++i) {
                currentNodeRank = currentNode->getRank();

                otherNodeWithSameRank = rankRegistry[currentNodeRank];
                while (otherNodeWithSameRank != 0) {
                    //C'è già un nodo con lo stesso rango. Merge!
                    --rootsCount;
                    --i;

                    //Il rango del nuovo nodo sarà uguale al rango di uno dei due
                    //nodi che stiamo considerando + 1. Quindi non ci sarà più nessun
                    //nodo con rango pari a quello attuale, e quindi bisogna vuotare la
                    //casella corrispondente nell'array
                    rankRegistry[currentNodeRank] = 0;

                    if (currentNode->getValue() <= otherNodeWithSameRank->getValue()) {
                        //Caso facile: l'altro nodo va aggiunto come figlio al nodo
                        //corrente
                        otherNodeWithSameRank->removeThisNodeFromSiblingsList();
                        currentNode->addChild(otherNodeWithSameRank);

                        //Attenzione: potrebbe succedere di prendere il nodo
                        //minimo e di metterlo come figlio di un altro nodo. Questa
                        //eventualità è il motivo per cui si usa un controllo con
                        //<= per aggiornare il minimo dopo questo while
                    } else {
                        //Attenzione: otherNodeWithSameRank potrebbe non essere
                        //nella posizione che stiamo considerando nella lista dei
                        //nodi. Quindi lo dobbiamo portare nella posizione corrente
                        insertBefore = currentNode->getNextSibling();

                        currentNode->removeThisNodeFromSiblingsList();
                        otherNodeWithSameRank->addChild(currentNode);

                        //Occhio al caso limite
                        if (insertBefore != otherNodeWithSameRank) {
                            otherNodeWithSameRank->removeThisNodeFromSiblingsList();
                            insertBefore->addSiblingBeforeNoCheck(otherNodeWithSameRank);
                        }

                        //Il nodo corrente diventa il nodo a cui abbiamo aggiunto un
                        //figlio, dato che lo abbiamo spostato nella posizione in cui
                        //prima c'era il nodo corrente
                        currentNode = otherNodeWithSameRank;
                    }

                    //Aggiorna il rango del nodo corrente. Dato che il nodo
                    //corrente è quello a cui è stato aggiunto un figlio, il suo
                    //rango sarà aumentato di 1
                    ++currentNodeRank;

                    //Verifica se è presente qualche altro nodo con lo stesso rango
                    otherNodeWithSameRank = rankRegistry[currentNodeRank];
                }

                //Nessun altro nodo con lo stesso rango. Salva il nodo corrente nell'array
                rankRegistry[currentNodeRank] = currentNode;

                //Verifica se serve aggiornare il nodo minimo

                //Si usa <= invece di < per gestire il caso in cui il minimo
                //globale sia già stato trovato in precedenza e poi sia stato
                //messo come figlio di un altro nodo con lo stesso valore
                if (currentNode->getValue() <= minNode->getValue()) {
                    minNode = currentNode;
                }

                //Avanza al nodo successivo
                currentNode = currentNode->getNextSibling();
            }
        }

#ifdef SANITY_CHECK
        checkHeapSanity();
#endif
    }

    void decrease(FibonacciHeapNode<T>* node, int newValue) {
        node->setValue(newValue);

        FibonacciHeapNode<T>* parent = node->getParent();
        if (parent != 0 && parent->getValue() > newValue) {
            //Proprietà dell'heap violata. Correggi
            FibonacciHeapNode<T>* currentNode = node;
            do {
                currentNode->moveNodeToRootList();
                //CurrentNode aveva un parent, quindi non poteva essere nella
                //lista delle radici e quindi non c'è bisogno di fare dei controlli
                //per verificare di non inserirlo prima del nodo che lo segue
                minNode->addSiblingBeforeNoCheck(currentNode);
                currentNode->unmark();
                ++rootsCount;

                currentNode = parent;
                parent = currentNode->getParent();
            } while (parent != 0 && currentNode->isMarked());

            //Non marcare il parent se questo è una radice
            if (parent != 0) {
                currentNode->mark();
            }
        }

        //Correggi il puntatore al nodo minimo se necessario
        if (newValue < minNode->getValue()) {
            minNode = node;
        }

#ifdef SANITY_CHECK
        checkHeapSanity();
#endif
    }

    FibonacciHeapNode<T>* peek() const {
        return minNode;
    }
private:
    FibonacciHeapNode<T>* minNode;
    int nodeCount;
    int rootsCount;

    FibonacciHeapNode<T>* getEmptyNode() {
        return new FibonacciHeapNode<T>();
    }

    void deleteNode(FibonacciHeapNode<T>* toDelete) {
        delete toDelete;
    }

    FibonacciHeapNode<T>** getNodeArrayWithAtLeastNElements(int n) {
        FibonacciHeapNode<T>** result = new FibonacciHeapNode<T>*[n];
        memset(result, 0, n * sizeof (FibonacciHeapNode<T>*));
        return result;
    }

    void deleteNodeArrayWithAtLeastNElements(FibonacciHeapNode<T>** toDelete) {
        delete[] toDelete;
    }

#ifdef SANITY_CHECK

    void checkHeapSanity() {
        if (rootsCount > 0) {
            std::vector<FibonacciHeapNode<T>*> nodes;

            FibonacciHeapNode<T>* firstNode = minNode;
            nodes.push_back(minNode);

            FibonacciHeapNode<T>* currentNode = minNode->getNextSibling();
            int count = 1;
            int allNodesCount = firstNode->checkNodeSanity();
            while (currentNode != firstNode && count < rootsCount) {
                if (currentNode->getValue() < minNode->getValue()) {
                    throw "Found another root with a smaller value than the min root!";
                }
                if (currentNode->getParent() != 0) {
                    throw "Root node does have a non-null parent";
                }
                nodes.push_back(currentNode);
                allNodesCount += currentNode->checkNodeSanity();
                ++count;

                currentNode = currentNode->getNextSibling();
            }

            if (currentNode != firstNode) {
                throw "Roots list is longer than the recorded length";
            } else if (count < rootsCount) {
                throw "Roots list is shorter than the recorded length";
            } else if (allNodesCount != nodeCount) {
                throw "Wrong number of nodes stored in heap";
            }

            currentNode = firstNode->getPrevSibling();
            int index = rootsCount - 1;
            count = 1;
            while (currentNode != firstNode && count < rootsCount) {
                if (nodes.at(index--) != currentNode) {
                    throw "Different sequence when going backward";
                }
                ++count;
                currentNode = currentNode->getPrevSibling();
            }

            if (currentNode != firstNode) {
                throw "Roots list is longer than the recorded length when going in reverse";
            } else if (count < rootsCount) {
                throw "Roots list is shorter than the recorded length when going in reverse";
            }
        }
    }
#endif

    FibonacciHeap(const FibonacciHeap& orig);
    FibonacciHeap& operator=(const FibonacciHeap& rhs);
};

template<typename T>
class PriorityQueue {
    friend PriorityQueueItem<T>;
public:

    PriorityQueue() {
        heap = new FibonacciHeap<PriorityQueueItem<T> >();
    }

    bool empty() const {
        return heap->empty();
    }

    int count() const {
        return heap->count();
    }

    void clear() {
        heap->clear();
    }

    PriorityQueueItem<T>* insert(T* element, int priority) {
        FibonacciHeapNode<PriorityQueueItem<T> >* newNode = heap->insert(priority);
        return getNewItem(element, newNode);
    }

    void decreaseKey(PriorityQueueItem<T>* elem, int newPriority) {
        heap->decrease(elem->heapNode, newPriority);
    }

    PriorityQueueItem<T>* peek() const {
        return heap->peek()->getPayload();
    }

    void deleteMin() {
        //Bisogna cancellare l'item della priority queue
        deleteItem(heap->peek()->getPayload());
        heap->deleteMin();
    }

    virtual ~PriorityQueue() {
        clear();
        delete heap;
    }
private:
    FibonacciHeap<PriorityQueueItem<T> >* heap;

    PriorityQueueItem<T>* getNewItem(T* payload, FibonacciHeapNode<PriorityQueueItem<T> >* node) {
        return new PriorityQueueItem<T>(payload, node);
    }

    void deleteItem(PriorityQueueItem<T>* item) {
        delete item;
    }

    PriorityQueue& operator=(const PriorityQueue& rhs);
    PriorityQueue(const PriorityQueue& orig);
};

GraphNode<Payload>* Labyrinth::goToNextIntersectionAndCreateNodeIfItDoesntExist(GraphNode<Payload>* node, int x, int y) {
    int initialDirectionIndex = 2 + (y - node->getY()) + ((x - node->getX()) * 2);
    int prevPositionIndex;
    int length = navigateToNextIntersection(node, x, y, prevPositionIndex);

    //Arrivati qui siamo in un incrocio o in un vicolo cieco
    //Verifica se esiste già il nodo corrispondente
    GraphNode<Payload>* destinationNode = nodes[y * columns + x];
    bool destinationNodeWasCreated = (destinationNode == 0);
    if (destinationNodeWasCreated) {
        destinationNode = new GraphNode<Payload>(x, y);
        Payload* p = new Payload(destinationNode);
        destinationNode->setPayload(p);
        nodes[y * columns + x] = destinationNode;
    }

    GraphEdge<Payload>* edge = new GraphEdge<Payload>(*node, *destinationNode, length);

    switch (initialDirectionIndex) {
        case 0:
            node->setW(edge);
            break;
        case 1:
            node->setN(edge);
            break;
            //Salto il 2
        case 3:
            node->setS(edge);
            break;
        case 4:
            node->setE(edge);
            break;
    }

    switch (prevPositionIndex) {
        case 0:
            destinationNode->setW(edge);
            break;
        case 1:
            destinationNode->setN(edge);
            break;
            //Salto il 2
        case 3:
            destinationNode->setS(edge);
            break;
        case 4:
            destinationNode->setE(edge);
            break;
    }

    if (destinationNodeWasCreated) {
        return destinationNode;
    }
    return 0;
}

int Labyrinth::navigateToNextIntersection(GraphNode<Payload>* node, int &x, int &y, int &prevPositionIndex, void (Labyrinth::*cellCallback)(int, int)) {
    int dirX = x - node->getX();
    int dirY = y - node->getY();

    char w = input[y * (columns + 1) + x - 1];
    char n = input[(y - 1) * (columns + 1) + x];
    char s = input[(y + 1) * (columns + 1) + x];
    char e = input[y * (columns + 1) + x + 1];
    
    bool emptyCells[5] = {
        (w == ' ' || w == '+'),
        (n == ' ' || n == '+'),
        false,
        (s == ' ' || s == '+'),
        (e == ' ' || e == '+')
    };

    int initialDirectionIndex = 2 + dirY + (dirX * 2);
    prevPositionIndex = 4 - initialDirectionIndex;
    int nextPositionIndex;

    int length = 1;
    while ((emptyCells[0] + emptyCells[1] + emptyCells[3] + emptyCells[4]) == 2) {

        if (cellCallback != 0) {
            (this->*cellCallback)(x, y);
        }

        //prevPositionIndex = 2 + (prevY - y) + ((prevX - x) * 2);
        for (nextPositionIndex = 4; nextPositionIndex >= 0; --nextPositionIndex) {
            if (nextPositionIndex == prevPositionIndex) {
                continue;
            }
            if (emptyCells[nextPositionIndex] == true) {
                break;
            }
        }

        //Aggiorna il prevPositionIndex per l'iterazione successiva
        prevPositionIndex = 4 - nextPositionIndex;
        //"centra" il nextPositionIndex per poter estrarre la direzione in cui
        //si sta andando
        nextPositionIndex -= 2;

        dirX = nextPositionIndex / 2;
        dirY = (nextPositionIndex - (dirX << 1));

        x += dirX;
        y += dirY;

        ++length;

        //Aggiorna le celle vuote attorno alla nuova posizione
        w = input[y * (columns + 1) + x - 1];
        n = input[(y - 1) * (columns + 1) + x];
        s = input[(y + 1) * (columns + 1) + x];
        e = input[y * (columns + 1) + x + 1];
        
        emptyCells[0] = (w == ' ' || w == '+');
        emptyCells[1] = (n == ' ' || n == '+');
        emptyCells[3] = (s == ' ' || s == '+');
        emptyCells[4] = (e == ' ' || e == '+');
    }

    return length;
}

Labyrinth::Labyrinth(char* stringInput, const int rows, const int columns) : input(stringInput), rows(rows), columns(columns) {
    int maxNodeCount = rows * columns;
    nodes = new GraphNode<Payload>*[maxNodeCount];
    //Azzera tutti i puntatori
    memset(nodes, 0, maxNodeCount * sizeof (GraphNode<Payload>*));

    const int lastRowOffset = (rows - 1) * (columns + 1);

    int upperExit = -1;
    int lowerExit = -1;

    for (int i = columns - 1; i >= 0; --i) {
        if (input[i] == ' ') {
            upperExit = i;
        }
        if (input[lastRowOffset + i] == ' ') {
            lowerExit = i;
        }
    }

    upperExitNode = new GraphNode<Payload>(upperExit, 0);
    Payload* p = new Payload(upperExitNode);
    upperExitNode->setPayload(p);
    nodes[upperExit] = upperExitNode;

    lowerExitNode = new GraphNode<Payload>(lowerExit, rows - 1);
    p = new Payload(lowerExitNode);
    lowerExitNode->setPayload(p);
    nodes[columns * (rows - 1) + lowerExit] = lowerExitNode;

    std::stack<GraphNode<Payload>*>* nodeStack = new std::stack<GraphNode<Payload>*>();

    //Inizializza lo stack con i nodi raggiungibili dalle due uscite
    GraphNode<Payload>* nodeToPush = goToNextIntersectionAndCreateNodeIfItDoesntExist(upperExitNode, upperExit, 1);
    if (nodeToPush != 0) {
        nodeStack->push(nodeToPush);
    }

    nodeToPush = goToNextIntersectionAndCreateNodeIfItDoesntExist(lowerExitNode, lowerExit, rows - 2);
    if (nodeToPush != 0) {
        nodeStack->push(nodeToPush);
    }

    while (!nodeStack->empty()) {
        GraphNode<Payload>* node = nodeStack->top();
        nodeStack->pop();

        //Considera tutte le direzioni in cui il nodo non ha già un edge settato
        //e per cui il carattere corrispondente nell'input è uno spazio
        int nodeX = node->getX();
        int nodeY = node->getY();

        int x = nodeX;
        int y = nodeY - 1;
        if (node->getN() == 0 && input[y * (columns + 1) + x] == ' ') {
            nodeToPush = goToNextIntersectionAndCreateNodeIfItDoesntExist(node, x, y);
            if (nodeToPush != 0) {
                nodeStack->push(nodeToPush);
            }
        }

        y = nodeY + 1;
        if (node->getS() == 0 && input[y * (columns + 1) + x] == ' ') {
            nodeToPush = goToNextIntersectionAndCreateNodeIfItDoesntExist(node, x, y);
            if (nodeToPush != 0) {
                nodeStack->push(nodeToPush);
            }
        }

        x = nodeX - 1;
        y = nodeY;
        if (node->getW() == 0 && input[y * (columns + 1) + x] == ' ') {
            nodeToPush = goToNextIntersectionAndCreateNodeIfItDoesntExist(node, x, y);
            if (nodeToPush != 0) {
                nodeStack->push(nodeToPush);
            }
        }

        x = nodeX + 1;
        if (node->getE() == 0 && input[y * (columns + 1) + x] == ' ') {
            nodeToPush = goToNextIntersectionAndCreateNodeIfItDoesntExist(node, x, y);
            if (nodeToPush != 0) {
                nodeStack->push(nodeToPush);
            }
        }
    }

    delete nodeStack;
}

void Labyrinth::drawSolution() {
    PriorityQueue<Payload>* pq = new PriorityQueue<Payload>();

    getStartNode()->getPayload()->setDistance(0);
    PriorityQueueItem<Payload>* pqi = pq->insert(
            getStartNode()->getPayload(),
            heuristic(getStartNode()->getX(), getStartNode()->getY())
            );
    pqi->getPayload()->setPriorityQueueItem(pqi);
    Payload* payload;

    GraphNode<Payload>* currentNode;
    
    int i = 0;

    while (!pq->empty()) {
        //Estrai l'elemento con la distanza totale stimata più bassa
        payload = pq->peek()->getPayload();
        pq->deleteMin();
        payload->setPriorityQueueItem(0);
        
        //Aggiorna la distanza reale (questo mette il nodo nel ClosedSet))
        payload->setDistance(payload->getTentativeDistance());

        currentNode = payload->getNode();
        if (currentNode == getEndNode()) {
            //Arrivati
            pq->clear();
            delete pq;
            outputSolution();
            return;
        }

        if (currentNode->getN() != 0) {
            processNodeAtTheOtherEndOfEdge(currentNode, currentNode->getN(), pq);
        }

        if (currentNode->getE() != 0) {
            processNodeAtTheOtherEndOfEdge(currentNode, currentNode->getE(), pq);
        }

        if (currentNode->getS() != 0) {
            processNodeAtTheOtherEndOfEdge(currentNode, currentNode->getS(), pq);
        }

        if (currentNode->getW() != 0) {
            processNodeAtTheOtherEndOfEdge(currentNode, currentNode->getW(), pq);
        }
        
        ++i;
    }

    pq->clear();

    delete pq;

    std::cout << "No solution" << std::endl;
    std::cout << input << std::endl;
}

void Labyrinth::processNodeAtTheOtherEndOfEdge(const GraphNode<Payload>* currentNode, const GraphEdge<Payload>* edge, PriorityQueue<Payload>* pq) const {
    GraphNode<Payload>* otherNode = getNodeAtTheOtherEndOfEdge(currentNode, edge);
    Payload* otherNodePayload = otherNode->getPayload();
    if (otherNodePayload->isInClodesSet()) {
        //Nodo già elaborato, niente da fare
        return;
    }

    int newTentativeDistance = currentNode->getPayload()->getDistance() + edge->getWeight();
    PriorityQueueItem<Payload>* otherNodePriorityQueueItem = otherNodePayload->getPriorityQueueItem();
    if (otherNodePriorityQueueItem == 0) {
        //L'altro nodo non è ancora in coda. Aggiungilo
        PriorityQueueItem<Payload>* pqi = pq->insert(
                otherNodePayload,
                newTentativeDistance + heuristic(otherNode->getX(), otherNode->getY())
                );
        pqi->getPayload()->setPriorityQueueItem(pqi);
    } else if (newTentativeDistance >= otherNodePriorityQueueItem->getPayload()->getTentativeDistance()) {
        //Questo è un percorso peggiore di quello già in coda
        return;
    } else {
        //Trovato un percorso migliore. Aggiorna la priorità nella coda
        pq->decreaseKey(otherNodePriorityQueueItem, newTentativeDistance + heuristic(otherNode->getX(), otherNode->getY()));
    }

    //Salva la nuova distanza provvisoria più bassa per raggiungere questo nodo
    otherNodePayload->setTentativeDistance(newTentativeDistance);

    //Aggiorna anche il nodo di provenienza per questo percorso
    otherNodePayload->setCamePromEdge(edge);
}

GraphNode<Payload>* Labyrinth::getNodeAtTheOtherEndOfEdge(const GraphNode<Payload>* currentNode, const GraphEdge<Payload>* edge) const {
    return reinterpret_cast<GraphNode<Payload>*> (
            reinterpret_cast<uintptr_t> (&edge->getFirst()) +
            reinterpret_cast<uintptr_t> (&edge->getSecond()) -
            reinterpret_cast<uintptr_t> (currentNode)
            );
}

int Labyrinth::heuristic(int x, int y) const {
    return abs(x - getEndNode()->getX()) + abs(y - getEndNode()->getY());
}

void Labyrinth::outputSolution() {
    GraphNode<Payload>* node = getEndNode();
    input[node->getY()*(columns + 1) + node->getX()] = '+';

    const GraphEdge<Payload>* edge = node->getPayload()->getCameFromEdge();

    while (edge != 0) {
        drawPath(node, edge);
        node = getNodeAtTheOtherEndOfEdge(node, edge);
        input[node->getY() * (columns + 1) + node->getX()] = '+';

        edge = node->getPayload()->getCameFromEdge();
    }

    std::cout << input;
}

void Labyrinth::drawPath(GraphNode<Payload>* node, const GraphEdge<Payload>* edge) {
    int x = node->getX();
    int y = node->getY();
    int dirX = 0;
    int dirY = 0;

    if (node->getN() == edge) {
        --dirY;
    } else if (node->getE() == edge) {
        ++dirX;
    } else if (node->getS() == edge) {
        ++dirY;
    } else if (node->getW() == edge) {
        --dirX;
    } else {
        throw "Unknown edge";
    }

    x += dirX;
    y += dirY;

    //Ora x ed x fanno riferimento alla prima casella nella direzione in cui ci
    //si deve spostare
    int prevPositionIndex;
    navigateToNextIntersection(node, x, y, prevPositionIndex, &Labyrinth::drawX);
}

void Labyrinth::drawX(int x, int y) {
    input[y * (columns + 1) + x] = '+';
}

GraphNode<Payload>* Labyrinth::getStartNode() const {
    return lowerExitNode;
}

GraphNode<Payload>* Labyrinth::getEndNode() const {
    return upperExitNode;
}

Labyrinth::~Labyrinth() {
    for (int i = rows * columns - 1; i >= 0; --i) {
        if (nodes[i] != 0) {
            delete nodes[i];
        }
    }
    delete[] nodes;
}




int main(int argc, char** argv) {

    char* inputFileName;
    if (argc >= 2) {
        inputFileName = argv[1];
    } else {
        //inputFileName = const_cast<char*> ("test.txt");
        inputFileName = const_cast<char*> ("test2.txt");
        //inputFileName = const_cast<char*> ("noSolution.txt");
    }

    std::ifstream infile(inputFileName, infile.binary | infile.ate);
    int fileSize = infile.tellg();
    infile.seekg(0, infile.beg);
    
    char* input = new char[fileSize + 1];
    infile.read(input, fileSize);
    
    while (infile.tellg() < fileSize && !infile.eof() && !infile.bad() && !infile.fail()) {
        infile.read(input + infile.tellg(), fileSize - infile.tellg());
    }
    
    if (infile.tellg() < fileSize) {
        std::cout << "Error while reading input file" << std::endl;
        return 1;
    }
    
    //Aggiungi il null
    input[fileSize] = 0;
    
    const int newlineLength = 1;
    
    char* pos = strchr(input, '\n');
    if (pos == 0) {
        std::cout << "Wrong input format" << std::endl;
        return 1;
    }
    
    int rowLength = pos - input + newlineLength;
    
    int rows = fileSize / rowLength;
    int columns = rowLength - newlineLength;
    
    if (((columns + newlineLength) * rows) != fileSize) {
        std::cout << "columns and rows count do not match with file size" << std::endl;
        return 1;
    }
    
    //Input letto. Crea il grafo
    Labyrinth lab(input, rows, columns);

    lab.drawSolution();

    delete[] input;
    input = 0;

    return 0;
}