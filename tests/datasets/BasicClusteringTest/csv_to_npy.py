import sys
import numpy as np
import os

def csv_to_npy(csv_filename):
    # Load CSV file into NumPy array
    data = np.loadtxt(csv_filename, delimiter=',')
    
    # Create output filename with same base name but .npy extension
    base_name = os.path.splitext(csv_filename)[0]
    npy_filename = base_name + '.npy'
    
    # Save the array to .npy file
    np.save(npy_filename, data)
    print(f"Saved: {npy_filename}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python csv_to_npy.py <input.csv>")
        sys.exit(1)
    
    csv_to_npy(sys.argv[1])
