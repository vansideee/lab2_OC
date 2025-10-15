#include <gtest/gtest.h>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <windows.h>
#include "CMakeLab2.cpp" 

// Тесты для функции ReadInteger
TEST(ReadIntegerTest, ValidInput) {
    std::stringstream input("5\n");
    std::cin.rdbuf(input.rdbuf()); // Перенаправляем std::cin
    int result = ReadInteger("Enter value: ", 0, 10);
    EXPECT_EQ(result, 5);
    std::cin.rdbuf(std::cin.rdbuf()); // Восстанавливаем std::cin
}

TEST(ReadIntegerTest, InputOutOfRange) {
    std::stringstream input("15\n5\n");
    std::cin.rdbuf(input.rdbuf());
    int result = ReadInteger("Enter value: ", 0, 10);
    EXPECT_EQ(result, 5); // Первая попытка вне диапазона, вторая валидна
    std::cin.rdbuf(std::cin.rdbuf());
}

TEST(ReadIntegerTest, InvalidInput) {
    std::stringstream input("abc\n5\n");
    std::cin.rdbuf(input.rdbuf());
    int result = ReadInteger("Enter value: ", 0, 10);
    EXPECT_EQ(result, 5); // Первая попытка невалидна, вторая валидна
    std::cin.rdbuf(std::cin.rdbuf());
}

TEST(ReadIntegerTest, TooManyAttempts) {
    std::stringstream input("abc\nabc\nabc\n");
    std::cin.rdbuf(input.rdbuf());
    EXPECT_THROW(ReadInteger("Enter value: ", 0, 10), std::invalid_argument);
    std::cin.rdbuf(std::cin.rdbuf());
}

// Тесты для функции CalculateResults
TEST(CalculateResultsTest, NormalArray) {
    std::vector<int> array = { 3, 1, 4, 1, 5 };
    CalculationResults results = CalculateResults(array);
    EXPECT_DOUBLE_EQ(results.average, 2.8); // (3 + 1 + 4 + 1 + 5) / 5 = 2.8
    EXPECT_EQ(results.minIndex, 1); // Минимальное значение 1 на индексе 1
    EXPECT_EQ(results.maxIndex, 4); // Максимальное значение 5 на индексе 4
}




TEST(CalculateResultsTest, SingleElement) {
    std::vector<int> array = { 42 };
    CalculationResults results = CalculateResults(array);
    EXPECT_DOUBLE_EQ(results.average, 42.0);
    EXPECT_EQ(results.minIndex, 0);
    EXPECT_EQ(results.maxIndex, 0);
}

// Тесты для MinMaxThreadProc
TEST(MinMaxThreadTest, ValidArray) {
    ThreadData data;
    data.array = { 3, 1, 4, 1, 5 };
    data.consoleMutex = CreateMutex(NULL, FALSE, NULL);
    ASSERT_NE(data.consoleMutex, NULL);

    DWORD result = MinMaxThreadProc(&data);
    EXPECT_EQ(result, 0); // Успешное выполнение

    CloseHandle(data.consoleMutex);
}

TEST(MinMaxThreadTest, EmptyArray) {
    ThreadData data;
    data.array = {};
    data.consoleMutex = CreateMutex(NULL, FALSE, NULL);
    ASSERT_NE(data.consoleMutex, NULL);

    DWORD result = MinMaxThreadProc(&data);
    EXPECT_EQ(result, 2); // Ошибка: пустой массив

    CloseHandle(data.consoleMutex);
}

TEST(MinMaxThreadTest, NullData) {
    DWORD result = MinMaxThreadProc(nullptr);
    EXPECT_EQ(result, 1); // Ошибка: нулевой указатель
}

TEST(AverageThreadTest, EmptyArray) {
    ThreadData data;
    data.array = {};
    data.consoleMutex = CreateMutex(NULL, FALSE, NULL);
    ASSERT_NE(data.consoleMutex, NULL);

    DWORD result = AverageThreadProc(&data);
    EXPECT_EQ(result, 2); // Ошибка: пустой массив

    CloseHandle(data.consoleMutex);
}

