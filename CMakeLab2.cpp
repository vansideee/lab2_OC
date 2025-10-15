// Lab2.cpp 

#define NOMINMAX  
#include <windows.h>  
#include <iostream>   
#include <vector>     
#include <string>     
#include <stdexcept>  
#include <algorithm>  
#include <limits>     


using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::runtime_error;
using std::invalid_argument;
using std::to_string;


static const int MIN_MAX_SLEEP_MS = 7;      
static const int AVERAGE_SLEEP_MS = 12;     
static const int MAX_INPUT_ATTEMPTS = 3;    
static const DWORD THREAD_WAIT_TIMEOUT = INFINITE;  

// Структура для передачи данных в потоки 
struct ThreadData {
    vector<int> array;      
    HANDLE consoleMutex;  
};

// Структура для хранения результатов вычислений
struct CalculationResults {
    double average;
    int minIndex;
    int maxIndex;
};

// RAII-класс для хэндлов
class HandleRAII {
public:
    explicit HandleRAII(HANDLE handle) : m_handle(handle) {
        if (m_handle == NULL) {  // Проверка на ошибку
            throw runtime_error("Failed to create handle.");
        }
    }

    ~HandleRAII() {
        if (m_handle != NULL && m_handle != INVALID_HANDLE_VALUE) {
            CloseHandle(m_handle);  // Освобождение ресурса
        }
    }

    HANDLE get() const { return m_handle; }

    HandleRAII(const HandleRAII&) = delete;  // no копированиe
    HandleRAII& operator=(const HandleRAII&) = delete;

private:
    HANDLE m_handle;
};

// Функция потока min_max
DWORD WINAPI MinMaxThreadProc(LPVOID lpParam) {
    ThreadData* data = static_cast<ThreadData*>(lpParam); 
    if (data == nullptr) {
        return 1; 
    }

    
    if (data->array.empty()) {
        return 2;  
    }

    int minVal = data->array[0];
    int maxVal = data->array[0];
    for (size_t i = 1; i < data->array.size(); ++i) {
        if (data->array[i] < minVal) {
            minVal = data->array[i];
        }
        if (data->array[i] > maxVal) {
            maxVal = data->array[i];
        }
        Sleep(MIN_MAX_SLEEP_MS); 
    }

    // Синхронизация вывода 
    DWORD waitResult = WaitForSingleObject(data->consoleMutex, THREAD_WAIT_TIMEOUT);
    if (waitResult != WAIT_OBJECT_0) {
        return 3;  
    }

    cout << "мин значение: " << minVal << endl;  
    cout << "макс значение: " << maxVal << endl;

    ReleaseMutex(data->consoleMutex); 

    return 0;  // Успех
}

// average
DWORD WINAPI AverageThreadProc(LPVOID lpParam) {
    ThreadData* data = static_cast<ThreadData*>(lpParam);
    if (data == nullptr) {
        return 1;
    }

    if (data->array.empty()) {
        return 2;
    }

    long long sum = 0;
    for (int val : data->array) {
        sum += val;
    }
    Sleep(AVERAGE_SLEEP_MS);  

    double average = static_cast<double>(sum) / data->array.size();  

    //синх вывода
    DWORD waitResult = WaitForSingleObject(data->consoleMutex, THREAD_WAIT_TIMEOUT);
    if (waitResult != WAIT_OBJECT_0) {
        return 3;
    }

    cout << "Average value: " << average << endl;

    ReleaseMutex(data->consoleMutex);

    return 0;
}


int ReadInteger(const string& prompt, int minVal = INT_MIN, int maxVal = INT_MAX) {
    int value = 0;
    int attempts = 0;
    while (attempts < MAX_INPUT_ATTEMPTS) {
        cout << prompt;
        if (cin >> value) {
            if (value >= minVal && value <= maxVal) {
                return value;
            }
            else {
                cout << "за рамками. трайни еще." << endl;
            }
        }
        else {
            cout << "вводи целое!!!" << endl;
            cin.clear();  
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        ++attempts;
    }
    throw invalid_argument("слишком много брехни.");  
}

//функция для вычисления
CalculationResults CalculateResults(const vector<int>& array) {
    CalculationResults results;

    if (array.empty()) {
        results.average = 0.0;
        results.minIndex = -1;
        results.maxIndex = -1;
        return results;
    }

   
    long long sum = 0;
    for (int val : array) {
        sum += val;
    }
    results.average = static_cast<double>(sum) / array.size();

    
    int minVal = array[0];
    int maxVal = array[0];
    results.minIndex = 0;
    results.maxIndex = 0;

    for (size_t i = 1; i < array.size(); ++i) {
        if (array[i] < minVal) {
            minVal = array[i];
            results.minIndex = static_cast<int>(i);
        }
        if (array[i] > maxVal) {
            maxVal = array[i];
            results.maxIndex = static_cast<int>(i);
        }
    }

    return results;
}


int main() {
    setlocale(LC_ALL, "RU");
    try {  
        
        int size = ReadInteger("введи размер массива(+ инт): ", 1);

        
        vector<int> array;
        array.reserve(static_cast<size_t>(size)); 
        for (int i = 0; i < size; ++i) {
            int val = ReadInteger("введи элемент " + to_string(i + 1) + ": ");
            array.push_back(val);
        }

      
        HandleRAII consoleMutex(CreateMutex(NULL, FALSE, NULL));

       
        ThreadData minMaxData = { array, consoleMutex.get() };
        ThreadData averageData = { array, consoleMutex.get() };

       
        DWORD minMaxThreadId = 0;
        HandleRAII minMaxThread(CreateThread(NULL, 0, MinMaxThreadProc, &minMaxData, 0, &minMaxThreadId));
        if (minMaxThread.get() == NULL) {
            DWORD error = GetLastError(); 
            throw runtime_error("Ошибка создания потока: " + to_string(error));
        }

        DWORD averageThreadId = 0;
        HandleRAII averageThread(CreateThread(NULL, 0, AverageThreadProc, &averageData, 0, &averageThreadId));
        if (averageThread.get() == NULL) {
            DWORD error = GetLastError();
            throw runtime_error("Ошибка создания авг потока: " + to_string(error));
        }

        //ожидание обоих потоков 
        HANDLE threads[2] = { minMaxThread.get(), averageThread.get() };
        DWORD waitResult = WaitForMultipleObjects(2, threads, TRUE, THREAD_WAIT_TIMEOUT);
        if (waitResult == WAIT_FAILED) {
            DWORD error = GetLastError();
            throw runtime_error("Ожидание потоков!!!: " + to_string(error));
        }

        
        if (!array.empty()) {
           
            CalculationResults results = CalculateResults(array);

           
            vector<double> modifiedArray(array.begin(), array.end());

            
            modifiedArray[results.minIndex] = results.average;
            modifiedArray[results.maxIndex] = results.average;

           
            cout << "Модифицированный массив:" << endl;
            for (size_t i = 0; i < modifiedArray.size(); ++i) {
               
                if (modifiedArray[i] == static_cast<int>(modifiedArray[i])) {
                    cout << static_cast<int>(modifiedArray[i]);
                }
                else {
                    cout << modifiedArray[i];
                }

                if (i < modifiedArray.size() - 1) {
                    cout << " ";
                }
            }
            cout << endl;

            
            cout << "\nИнфа о заменах:" << endl;
            cout << "Элемент с инд " << results.minIndex << " (min) заменен на: " << results.average << endl;
            cout << "Элемент с инд " << results.maxIndex << " (max) заменен на: " << results.average << endl;

            
            cout << "\nmin value: " << array[results.minIndex] << endl;
            cout << "max value: " << array[results.maxIndex] << endl;
            cout << "avg исп для замены: " << results.average << endl;
        }

        cout << "\nВсе потоки получилися!" << endl;

    }
    catch (const std::exception& e) {
        cout << "ошибка: " << e.what() << endl;
        return 1;
    }

    return 0;
}