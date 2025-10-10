
import os
import numpy as np
import matplotlib.pyplot as plt

from voroclust import VoroClust

def main():
    # Specify algorithm hyperparameters
    R = 0.25
    detail_ceiling = 0.8
    descent_limit = 0.1
    NUM_THREADS = 8


    # Specify optional post-processing options
    noise_style = "Assign Noise"
    max_clusters = None
    noise_threshold = 0.05

    # Initialize clustering model
    data = np.load("./datasets/BasicClusteringTest/noisy_moons.npy")
    model = VoroClust(data,
                      radius=R,
                      detail_ceiling=detail_ceiling,
                      descent_limit=descent_limit,
                      num_threads=NUM_THREADS)

    # Alternative option; loading from file
    #model = VoroClust(data_filename="./datasets/BasicClusteringTest/noisy_moons.csv",
    #                  radius=R,
    #                  detail_ceiling=detail_ceiling,
    #                  descent_limit=descent_limit,
    #                  num_threads=NUM_THREADS)

    # Fit clustering model to data
    cluster_vals, labels, noise_indices = model.fit(noise_style=noise_style,
                                                    max_clusters=max_clusters,
                                                    noise_threshold=noise_threshold)

    # Plot results
    model.plot_predictions(cmap="tab20c")


if __name__ == "__main__":
    main()
