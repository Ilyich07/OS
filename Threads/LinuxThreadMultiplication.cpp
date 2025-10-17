#include <pthread.h>
#include <iostream>
#include <vector>
#include <memory>
#include <chrono>
#include <cstdlib>


class Matrix {
private:
    std::vector<std::vector<int>> data;
    int rows, cols;

public:
    Matrix(int n) : rows(n), cols(n), data(n, std::vector<int>(n, 0)) {}
    Matrix(int rows, int cols) : rows(rows), cols(cols), data(rows, std::vector<int>(cols, 0)) {}

    int getRows() const { return rows; }
    int getCols() const { return cols; }

    int& operator()(int i, int j) { return data[i][j]; }
    const int& operator()(int i, int j) const { return data[i][j]; }

    void fillRandom() {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                data[i][j] = rand() % 10;
            }
        }
    }

    void print() const {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                std::cout << data[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
};

class Block {
private:
    std::vector<std::vector<int>> data;
    int startRow, startCol;
    int actualRows, actualCols;

public:
    Block(int startRow, int startCol, int blockSize, int matrixSize) 
        : startRow(startRow), startCol(startCol) {
        actualRows = (startRow + blockSize <= matrixSize) ? blockSize : matrixSize - startRow;
        actualCols = (startCol + blockSize <= matrixSize) ? blockSize : matrixSize - startCol;
        data.resize(actualRows, std::vector<int>(actualCols, 0));
    }

    int getStartRow() const { return startRow; }
    int getStartCol() const { return startCol; }
    int getActualRows() const { return actualRows; }
    int getActualCols() const { return actualCols; }

    int& operator()(int i, int j) { return data[i][j]; }
    const int& operator()(int i, int j) const { return data[i][j]; }

    void fillFromMatrix(const Matrix& matrix) {
        for (int i = 0; i < actualRows; ++i) {
            for (int j = 0; j < actualCols; ++j) {
                int row = startRow + i;
                int col = startCol + j;
                data[i][j] = matrix(row, col);
            }
        }
    }

    void addToMatrix(Matrix& matrix) const {
        for (int i = 0; i < actualRows; ++i) {
            for (int j = 0; j < actualCols; ++j) {
                int row = startRow + i;
                int col = startCol + j;
                matrix(row, col) += data[i][j];
            }
        }
    }

    void clear() {
        for (int i = 0; i < actualRows; ++i) {
            for (int j = 0; j < actualCols; ++j) {
                data[i][j] = 0;
            }
        }
    }
};

class BlockMatrixMultiplier {
private:
    int matrixSize;
    int blockSize;
    Matrix A, B, C;
    pthread_mutex_t resultMutex;
    int countOfthreads;

    struct ThreadArgs {
        BlockMatrixMultiplier* multiplier;
        int blockRowA;
        int blockColA;
        int blockRowB;
        int blockColB;
    };

    static void* threadWrapper(void* arg) {
        ThreadArgs* args = static_cast<ThreadArgs*>(arg);
        args->multiplier->processBlockPair(
            args->blockRowA, args->blockColA, 
            args->blockRowB, args->blockColB
        );
        delete args;
        return nullptr;
    }

    void multiplyBlocks(const Block& blockA, const Block& blockB, Block& resultBlock) {
        int rowsA = blockA.getActualRows();
        int colsA = blockA.getActualCols();
        int rowsB = blockB.getActualRows();
        int colsB = blockB.getActualCols();
        
        int commonDim = std::min(colsA, rowsB);
        
        for (int i = 0; i < rowsA; ++i) {
            for (int k = 0; k < commonDim; ++k) {
                for (int j = 0; j < colsB; ++j) {
                    resultBlock(i, j) += blockA(i, k) * blockB(k, j);
                }
            }
        }
    }

    void processBlockPair(int blockRowA, int blockColA, int blockRowB, int blockColB) {
        int startRowA = blockRowA * blockSize;
        int startColA = blockColA * blockSize;
        int startRowB = blockRowB * blockSize;
        int startColB = blockColB * blockSize;
        
        Block blockA(startRowA, startColA, blockSize, matrixSize);
        Block blockB(startRowB, startColB, blockSize, matrixSize);
        Block tempResult(startRowA, startColB, blockSize, matrixSize);
        
        blockA.fillFromMatrix(A);
        blockB.fillFromMatrix(B);
        tempResult.clear();

        multiplyBlocks(blockA, blockB, tempResult);

        pthread_mutex_lock(&resultMutex);
        tempResult.addToMatrix(C);
        pthread_mutex_unlock(&resultMutex);
    }

public:
    BlockMatrixMultiplier(int n, int bs) 
        : matrixSize(n), blockSize(bs), A(n), B(n), C(n), countOfthreads(0) {
        pthread_mutex_init(&resultMutex, nullptr);
    }

    ~BlockMatrixMultiplier() {
        pthread_mutex_destroy(&resultMutex);
    }

    void setMatrices(const Matrix& matrixA, const Matrix& matrixB) {
        A = matrixA;
        B = matrixB;
        for (int i = 0; i < matrixSize; ++i) {
            for (int j = 0; j < matrixSize; ++j) {
                C(i, j) = 0;
            }
        }
    }

    Matrix multiply() {
        int numBlocks = (matrixSize + blockSize - 1) / blockSize;
        std::vector<pthread_t> threads;
        std::vector<ThreadArgs*> threadArgs;
        countOfthreads = 0;

        for (int blockRowA = 0; blockRowA < numBlocks; ++blockRowA) {
            for (int blockColA = 0; blockColA < numBlocks; ++blockColA) {
                for (int blockColB = 0; blockColB < numBlocks; ++blockColB) {
                    int blockRowB = blockColA;

                    ThreadArgs* args = new ThreadArgs{
                        this, blockRowA, blockColA, blockRowB, blockColB
                    };
                    
                    pthread_t thread;
                    int result = pthread_create(&thread, nullptr, threadWrapper, args);
                    
                    if (result == 0) {
                        threads.push_back(thread);
                        threadArgs.push_back(args);
                        countOfthreads++;
                    } else {
                        delete args;
                        std::cerr << "Failed to create thread!" << std::endl;
                    }
                }
            }
        }

        for (pthread_t& thread : threads) {
            pthread_join(thread, nullptr);
        }

        return C;
    }

    int getActualThreadsCreated() const {
        return countOfthreads;
    }
};

Matrix multiplyMatricesSimple(const Matrix& A, const Matrix& B) {
    int n = A.getRows();
    Matrix C(n);
    
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            C(i, j) = 0;
            for (int k = 0; k < n; ++k) {
                C(i, j) += A(i, k) * B(k, j);
            }
        }
    }
    
    return C;
}

int main() {
    int N;
    
    do {
        std::cout << "Enter matrix size N (minimum 5): ";
        std::cin >> N;
        if (N < 5) {
            std::cout << "Error: N must be at least 5!" << std::endl;
        }
    } while (N < 5);

    Matrix A(N), B(N);
    srand(time(0));
    A.fillRandom();
    B.fillRandom();

    std::cout << "\nINPUT MATRICES" << std::endl;
    std::cout << "\nMatrix A:" << std::endl;
    A.print();
    std::cout << "\nMatrix B:" << std::endl;
    B.print();
    

    std::cout << "\nSIMPLE MULTIPLICATION ALGORITHM\n" << std::endl;

    auto startSimple = std::chrono::high_resolution_clock::now();
    Matrix resultSimple = multiplyMatricesSimple(A, B);
    auto endSimple = std::chrono::high_resolution_clock::now();
    long long simpleTimeMicro = std::chrono::duration_cast<std::chrono::microseconds>(endSimple - startSimple).count();
    double simpleTimeMs = simpleTimeMicro / 1000.0;
    std::cout << "Time: " << simpleTimeMs << " ms" << std::endl;

    std::cout << "\nBLOCK MULTIPLICATION ALGORITHM\n" << std::endl;

    for (int blockSize = 1; blockSize <= N; blockSize++) {
        BlockMatrixMultiplier multiplier(N, blockSize);
        multiplier.setMatrices(A, B);
        
        auto start = std::chrono::high_resolution_clock::now();
        Matrix resultBlock = multiplier.multiply();
        auto end = std::chrono::high_resolution_clock::now();
        
        long long time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        double timeMs = time / 1000.0;
        int countOfthreads = multiplier.getActualThreadsCreated();
        
        std::cout << "Block size: " << blockSize << std::endl;
        std::cout << "Threads created: " << countOfthreads << std::endl;
        std::cout << "Time: " << timeMs << " ms" << std::endl;
        
        // Verify result matches simple multiplication
        bool correct = true;
        for (int i = 0; i < N && correct; ++i) {
            for (int j = 0; j < N && correct; ++j) {
                if (resultBlock(i, j) != resultSimple(i, j)) {
                    correct = false;
                }
            }
        }
        std::cout << "Result correct: " << (correct ? "Yes" : "No") << "\n" << std::endl;
    }


    std::cout << "\nMultiplication result:" << std::endl;
    resultSimple.print();
}
