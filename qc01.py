from qiskit import QuantumCircuit
from qiskit_aer import Aer
from qiskit import transpile

# 1 qubit, 1 classical bit
qc = QuantumCircuit(1, 1)
qc.h(0)
qc.measure(0, 0)

backend = Aer.get_backend("aer_simulator")
tqc = transpile(qc, backend)
result = backend.run(tqc, shots=1000).result()
print(result.get_counts())
