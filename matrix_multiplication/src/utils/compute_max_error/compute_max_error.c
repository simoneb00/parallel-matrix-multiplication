float compute_max_error(float **matrix, int rows, int cols) {
    float max_error = 0.0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (matrix[i][j] > max_error)
                max_error = matrix[i][j];
        }
    }

    return max_error;
}