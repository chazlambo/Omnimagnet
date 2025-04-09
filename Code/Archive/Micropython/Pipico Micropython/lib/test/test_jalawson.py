import umatrix, ulinalg
import data as d
import show

A = umatrix.matrix(d.A)
B = umatrix.matrix(d.B).transpose()

print('=== Multiply Integer matrix ...')
X = show.time(ulinalg.dot)(A, B)
print(X)

C = umatrix.matrix(d.C)
D = umatrix.matrix(d.D).transpose()

print('=== Multiply Float matrix ...')
X = show.time(ulinalg.dot)(C, D)
print(X)
