#include <gtest/gtest.h>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <windows.h>
#include "CMakeLab2.cpp" 

// ����� ��� ������� ReadInteger
TEST(ReadIntegerTest, ValidInput) {
    std::stringstream input("5\n");
    std::cin.rdbuf(input.rdbuf()); // �������������� std::cin
    int result = ReadInteger("Enter value: ", 0, 10);
    EXPECT_EQ(result, 5);
    std::cin.rdbuf(std::cin.rdbuf()); // ��������������� std::cin
}

TEST(ReadIntegerTest, InputOutOfRange) {
    std::stringstream input("15\n5\n");
    std::cin.rdbuf(input.rdbuf());
    int result = ReadInteger("Enter value: ", 0, 10);
    EXPECT_EQ(result, 5); // ������ ������� ��� ���������, ������ �������
    std::cin.rdbuf(std::cin.rdbuf());
}

TEST(ReadIntegerTest, InvalidInput) {
    std::stringstream input("abc\n5\n");
    std::cin.rdbuf(input.rdbuf());
    int result = ReadInteger("Enter value: ", 0, 10);
    EXPECT_EQ(result, 5); // ������ ������� ���������, ������ �������
    std::cin.rdbuf(std::cin.rdbuf());
}

TEST(ReadIntegerTest, TooManyAttempts) {
    std::stringstream input("abc\nabc\nabc\n");
    std::cin.rdbuf(input.rdbuf());
    EXPECT_THROW(ReadInteger("Enter value: ", 0, 10), std::invalid_argument);
    std::cin.rdbuf(std::cin.rdbuf());
}

// ����� ��� ������� CalculateResults
TEST(CalculateResultsTest, NormalArray) {
    std::vector<int> array = { 3, 1, 4, 1, 5 };
    CalculationResults results = CalculateResults(array);
    EXPECT_DOUBLE_EQ(results.average, 2.8); // (3 + 1 + 4 + 1 + 5) / 5 = 2.8
    EXPECT_EQ(results.minIndex, 1); // ����������� �������� 1 �� ������� 1
    EXPECT_EQ(results.maxIndex, 4); // ������������ �������� 5 �� ������� 4
}




TEST(CalculateResultsTest, SingleElement) {
    std::vector<int> array = { 42 };
    CalculationResults results = CalculateResults(array);
    EXPECT_DOUBLE_EQ(results.average, 42.0);
    EXPECT_EQ(results.minIndex, 0);
    EXPECT_EQ(results.maxIndex, 0);
}

// ����� ��� MinMaxThreadProc
TEST(MinMaxThreadTest, ValidArray) {
    ThreadData data;
    data.array = { 3, 1, 4, 1, 5 };
    data.consoleMutex = CreateMutex(NULL, FALSE, NULL);
    ASSERT_NE(data.consoleMutex, NULL);

    DWORD result = MinMaxThreadProc(&data);
    EXPECT_EQ(result, 0); // �������� ����������

    CloseHandle(data.consoleMutex);
}

TEST(MinMaxThreadTest, EmptyArray) {
    ThreadData data;
    data.array = {};
    data.consoleMutex = CreateMutex(NULL, FALSE, NULL);
    ASSERT_NE(data.consoleMutex, NULL);

    DWORD result = MinMaxThreadProc(&data);
    EXPECT_EQ(result, 2); // ������: ������ ������

    CloseHandle(data.consoleMutex);
}

TEST(MinMaxThreadTest, NullData) {
    DWORD result = MinMaxThreadProc(nullptr);
    EXPECT_EQ(result, 1); // ������: ������� ���������
}

TEST(AverageThreadTest, EmptyArray) {
    ThreadData data;
    data.array = {};
    data.consoleMutex = CreateMutex(NULL, FALSE, NULL);
    ASSERT_NE(data.consoleMutex, NULL);

    DWORD result = AverageThreadProc(&data);
    EXPECT_EQ(result, 2); // ������: ������ ������

    CloseHandle(data.consoleMutex);
}

