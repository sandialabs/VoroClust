
import os
import numpy as np
import matplotlib.pyplot as plt

from voroclust import VoroClust

def main():
    # Specify algorithm hyperparameters
    R = 0.08
    detail_ceiling = 0.8
    descent_limit = 0.1
    NUM_THREADS = 8


    # Specify optional post-processing options
    noise_style = "Prune Clusters"
    max_clusters = 9
    noise_threshold = None

    # Initialize clustering model
    model = VoroClust(data_filename="datasets/BasicClusteringTest/example_image.png",
                      radius=R,
                      detail_ceiling=detail_ceiling,
                      descent_limit=descent_limit,
                      num_threads=NUM_THREADS)
    
    # Fit clustering model to data
    cluster_vals, labels, noise_indices = model.fit(noise_style=noise_style,
                                                    max_clusters=max_clusters,
                                                    noise_threshold=noise_threshold)

    # Plot results
    model.plot_predictions(cmap="tab20c")


if __name__ == "__main__":
    main()
