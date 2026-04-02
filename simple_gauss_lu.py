import numpy as np

def lu_decomposition(A):
    """
    对矩阵 A 进行 LU 分解（不选主元，假设所有主元非零）。
    返回 (L, U)，其中 L 是单位下三角矩阵，U 是上三角矩阵。
    """
    n = A.shape[0]
    # 拷贝一份，避免修改原矩阵
    U = A.astype(float).copy()
    L = np.eye(n)

    for k in range(n - 1):
        if abs(U[k, k]) < 1e-12:
            raise ValueError(f"零主元出现在第 {k} 步，无法进行 LU 分解（请尝试其他矩阵）")
        for i in range(k + 1, n):
            L[i, k] = U[i, k] / U[k, k]          # 记录乘数
            for j in range(k, n):
                U[i, j] -= L[i, k] * U[k, j]     # 消元

    return L, U

def verify_lu_decomposition(A, L, U, tol=1e-10):
    """验证 A = L @ U"""
    diff = np.abs(A - L @ U)
    max_err = np.max(diff)
    print(f"   A - L@U 的最大误差: {max_err:.2e}")
    return max_err < tol

def construct_M_k(L, k):
    """
    根据 L 的第 k 列乘数构造初等矩阵 M_k = I - l_k * e_k^T
    其中 l_k 是第 k 列下方的乘数向量（其余为 0）。
    """
    n = L.shape[0]
    l_k = np.zeros(n)
    l_k[k+1:] = L[k+1:, k]          # 乘数
    e_k = np.zeros(n)
    e_k[k] = 1
    M_k = np.eye(n) - np.outer(l_k, e_k)
    return M_k

def verify_elimination_matrices(A, L, U, tol=1e-10):
    """验证 M_{n-1} ... M_1 A = U，并验证 M 的逆等于 L"""
    n = A.shape[0]
    M = np.eye(n)
    # 按顺序左乘 M_1, M_2, ..., M_{n-1}
    for k in range(n - 1):
        M_k = construct_M_k(L, k)
        M = M_k @ M          # 左乘，顺序正确

    # 验证 M A = U
    MA = M @ A
    diff = np.abs(MA - U)
    max_err = np.max(diff)
    print(f"   M@A - U 的最大误差: {max_err:.2e}")

    # 验证 M 的逆等于 L
    M_inv = np.linalg.inv(M)
    diff_inv = np.abs(M_inv - L)
    max_err_inv = np.max(diff_inv)
    print(f"   M^{-1} - L 的最大误差: {max_err_inv:.2e}")

    # 验证 M 是单位下三角矩阵
    # np.triu(M, k=1)：获取矩阵 M 的上三角部分（不包括主对角线，k=1表示从主对角线以上的第一条对角线开始）
    # np.allclose(..., 0)：检查上三角部分是否全部接近于0（考虑浮点误差）
    # 是的，NumPy 中与 np.triu 对应的获取下三角矩阵的 API 是 np.tril （triangular lower 的缩写）。
    is_lower_tri = np.allclose(np.triu(M, k=1), 0)
    # np.diag(M)：获取矩阵 M 的主对角线元素
    # np.allclose(np.diag(M), 1)：检查主对角线元素是否全部接近于1（考虑浮点误差）
    diag_ones = np.allclose(np.diag(M), 1)
    print(f"   M 是单位下三角矩阵: {is_lower_tri and diag_ones}")

    return max_err < tol and max_err_inv < tol

def compact_storage_demo(A, L, U):
    """展示紧凑存储形式：L 的下三角（不含对角线）与 U 的上三角合并"""
    n = A.shape[0]
    compact = np.zeros_like(A, dtype=float)
    # 下三角部分（包括对角线）用 L 填充（实际 L 对角线为 1，但紧凑存储中通常不存）
    for i in range(n):
        for j in range(i+1):
            compact[i, j] = L[i, j] if i != j else 1.0   # 对角线用 1，但实际 U 对角线会覆盖
    # 上三角部分（包括对角线）用 U 填充
    for i in range(n):
        for j in range(i, n):
            compact[i, j] = U[i, j]
    print("紧凑存储矩阵（下三角为乘数，对角线及上三角为 U）:")
    print(compact)

def test_matrix(A, name):
    """对一个矩阵执行完整验证"""
    print(f"\n{'='*60}")
    print(f"测试矩阵: {name}")
    print(f"A =\n{A}")
    n = A.shape[0]
    try:
        L, U = lu_decomposition(A)
    except ValueError as e:
        print(e)
        return

    print("\n--- 1. 验证 LU 分解正确性 ---")
    ok1 = verify_lu_decomposition(A, L, U)
    print(f"   LU 分解正确: {ok1}")

    print("\n--- 2. 验证用初等矩阵表示消元过程 ---")
    ok2 = verify_elimination_matrices(A, L, U)

    print("\n--- 3. 紧凑存储形式 ---")
    compact_storage_demo(A, L, U)

    print("\n--- 4. 验证 L 元素就是乘数 ---")
    print("   L 矩阵（单位下三角）:")
    print(L)
    print("   下三角元素即为消元过程中的乘数 l_ij.")

def main():
    # 实例1：2x2 矩阵
    A1 = np.array([[2.0, 1.0],
                   [4.0, 3.0]])
    test_matrix(A1, "2x2 矩阵")

    # 实例2：3x3 矩阵（随机整数，保证可逆）
    np.random.seed(42)
    while True:
        A2 = np.random.randint(1, 10, size=(3, 3)).astype(float)
        if abs(np.linalg.det(A2)) > 1e-6:
            break
    test_matrix(A2, "随机 3x3 整数矩阵")

    # 实例3：4x4 矩阵（随机整数，保证可逆）
    while True:
        A3 = np.random.randint(1, 10, size=(4, 4)).astype(float)
        if abs(np.linalg.det(A3)) > 1e-6:
            break
    test_matrix(A3, "随机 4x4 整数矩阵")

    # 实例4：特殊矩阵（演示紧凑存储）
    A4 = np.array([[1.0, 2.0, 1.0],
                   [3.0, 8.0, 1.0],
                   [2.0, 5.0, 3.0]])
    test_matrix(A4, "特殊 3x3 矩阵")

if __name__ == "__main__":
    main()