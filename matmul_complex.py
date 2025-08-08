from typing import List

Matrix = List[List[complex]]

def matmul_complex(A: Matrix, B: Matrix) -> Matrix:
    # 维度检查
    assert len(A) > 0 and len(B) > 0, "Empty matrices not allowed"
    n, m, p = len(A), len(A[0]), len(B[0])
    assert all(len(row) == m for row in A), "A is not rectangular"
    assert all(len(row) == p for row in B), "B is not rectangular"
    assert len(B) == m, "Inner dimensions must match"

    C: Matrix = [[0+0j for _ in range(p)] for _ in range(n)]
    for i in range(n):
        for k in range(m):
            aik = A[i][k]
            for j in range(p):
                C[i][j] += aik * B[k][j]
    return C

if __name__ == "__main__":
    A = [[1+2j, 3-1j],
         [0+1j, -2+0j]]
    B = [[2-1j, 0+2j],
         [1+0j,  4-3j]]
    C = matmul_complex(A, B)
    print(C)
