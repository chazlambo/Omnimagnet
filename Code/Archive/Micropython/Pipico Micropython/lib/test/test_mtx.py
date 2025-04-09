import mtx, show
import data as d

def bulk_mul(A, B):
    return [mtx.mul(A, b) for b in B]

def bulk_solve(A, B):
    return [mtx.solve(A, b) for b in B]

print('=== Multiply two integer matrix ...')
X = show.time(bulk_mul)(d.A, d.B)
print(X)

print('=== Multiply two float matrix ...')
Y = show.time(bulk_mul)(d.C, d.D)
print(Y)

print('=== Multiply vector ...')
x = show.time(mtx.mul)(d.A, d.a)
print(x)

print('=== LU-factor integer matrix ...')
A_ = show.time(mtx.lu)([r[:] for r in d.A])

print('=== Recover B ...')
B_ = show.time(bulk_solve)(A_, X)
print(B_)

print('=== LU-factor float matrix ...')
C_ = show.time(mtx.lu)([r[:] for r in d.C])

print('=== Recover D ...')
D_ = show.time(bulk_solve)(C_, Y)
print(D_)

print('=== LU-Factor E ...')
E_ = show.time(mtx.lu)([r[:] for r in d.E])
print(E_)

print('=== Solve using intermediate form ...')
x = show.time(mtx.solve)(E_, d.b)
print(x)
