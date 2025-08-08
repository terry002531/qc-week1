import numpy as np
from matmul_complex import matmul_complex

def to_np(M):
    return np.array(M, dtype=np.complex128)

def test_small_case():
    A = [[1+2j, 3-1j],
         [0+1j, -2+0j]]
    B = [[2-1j, 0+2j],
         [1+0j,  4-3j]]

    C_py = to_np(matmul_complex(A,B))
    C_np = to_np(A) @ to_np(B)
    assert np.allclose(C_py, C_np)
