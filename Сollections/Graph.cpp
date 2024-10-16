#include "pch.h"
#include "gtest/gtest.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <vector>
#include <unordered_set>
#include <stdexcept>
#include <iostream>
#include <Windows.h> 

class Graph {
private:
    int vertices;
    std::vector<std::unordered_set<int>> adjacencyList;
    std::shared_ptr<spdlog::logger> logger;

public:
    // Конструктор с указанием количества вершин и логгера
    Graph(int v, std::shared_ptr<spdlog::logger> logger)
        : vertices(v), adjacencyList(v), logger(logger) {
        if (v <= 0) {
            throw std::invalid_argument("Количество вершин должно быть положительным");
        }
        logger->info("Граф создан с {} вершинами", v);
    }

    // Добавление ребра между вершинами v1 и v2
    void addEdge(int v1, int v2) {
        if (v1 >= vertices || v2 >= vertices || v1 < 0 || v2 < 0) {
            throw std::out_of_range("Вершина вне допустимого диапазона");
        }
        adjacencyList[v1].insert(v2);
        adjacencyList[v2].insert(v1);  // Граф неориентированный
        logger->info("Ребро добавлено между {} и {}", v1, v2);
        printAdjacencyMatrix();  // Выводим матрицу смежности после добавления ребра
    }

    // Удаление ребра между вершинами v1 и v2
    void removeEdge(int v1, int v2) {
        if (v1 >= vertices || v2 >= vertices || v1 < 0 || v2 < 0) {
            throw std::out_of_range("Вершина вне допустимого диапазона");
        }
        adjacencyList[v1].erase(v2);
        adjacencyList[v2].erase(v1);  // Граф неориентированный
        logger->info("Ребро удалено между {} и {}", v1, v2);
        printAdjacencyMatrix();  // Выводим матрицу смежности после удаления ребра
    }

    // Проверка наличия ребра между вершинами v1 и v2
    bool hasEdge(int v1, int v2) const {
        if (v1 >= vertices || v2 >= vertices || v1 < 0 || v2 < 0) {
            throw std::out_of_range("Вершина вне допустимого диапазона");
        }
        return adjacencyList[v1].find(v2) != adjacencyList[v1].end();
    }

    // Получить количество вершин
    int getVertices() const {
        return vertices;
    }

    // Получить список смежных вершин для заданной вершины
    const std::unordered_set<int>& getAdjacentVertices(int v) const {
        if (v >= vertices || v < 0) {
            throw std::out_of_range("Вершина вне допустимого диапазона");
        }
        return adjacencyList[v];
    }

    // Печать графа в виде матрицы смежности
    void printAdjacencyMatrix() const {
        std::cout << "\nМатрица смежности:\n";
        std::cout << "   ";
        for (int i = 0; i < vertices; ++i) {
            std::cout << i << " ";
        }
        std::cout << "\n";

        for (int i = 0; i < vertices; ++i) {
            std::cout << i << ": ";
            for (int j = 0; j < vertices; ++j) {
                if (adjacencyList[i].find(j) != adjacencyList[i].end()) {
                    std::cout << "1 ";
                }
                else {
                    std::cout << "0 ";
                }
            }
            std::cout << "\n";
        }
    }
};

// Тесты с использованием Google Test
class GraphTest : public ::testing::Test {
protected:
    std::shared_ptr<spdlog::logger> logger;
    Graph* graph;

    void SetUp() override {
        // Проверяем, существует ли логгер с именем "test_logger"
        logger = spdlog::get("test_logger");
        if (!logger) {
            logger = spdlog::stdout_color_mt("test_logger");
        }

        // Инициализируем граф с 5 вершинами
        graph = new Graph(5, logger);
    }

    void TearDown() override {
        delete graph;
        graph = nullptr;
    }
};


TEST_F(GraphTest, AddEdgeTest) {
    graph->addEdge(0, 1);
    EXPECT_TRUE(graph->hasEdge(0, 1));
    EXPECT_TRUE(graph->hasEdge(1, 0));
}

TEST_F(GraphTest, RemoveEdgeTest) {
    graph->addEdge(0, 1);
    graph->removeEdge(0, 1);
    EXPECT_FALSE(graph->hasEdge(0, 1));
    EXPECT_FALSE(graph->hasEdge(1, 0));
}

TEST_F(GraphTest, OutOfBoundsTest) {
    EXPECT_THROW(graph->addEdge(0, 5), std::out_of_range);
    EXPECT_THROW(graph->removeEdge(-1, 3), std::out_of_range);
}

TEST_F(GraphTest, HasEdgeTest) {
    graph->addEdge(2, 3);
    EXPECT_TRUE(graph->hasEdge(2, 3));
    EXPECT_TRUE(graph->hasEdge(3, 2));
    EXPECT_FALSE(graph->hasEdge(0, 3));
}

TEST_F(GraphTest, AdjacentVerticesTest) {
    graph->addEdge(2, 3);
    graph->addEdge(2, 4);
    const auto& adj = graph->getAdjacentVertices(2);
    EXPECT_EQ(adj.size(), 2);
    EXPECT_TRUE(adj.find(3) != adj.end());
    EXPECT_TRUE(adj.find(4) != adj.end());
}

int main(int argc, char** argv) {

    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    spdlog::set_level(spdlog::level::info);  // Устанавливаем уровень логгирования
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
