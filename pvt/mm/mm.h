void block_matrix_multiply(
	double* A, double* B, int a_rows, int b_cols, int ab_commn, int block_size, double* C)
{
	int a_height_blocks = a_rows / block_size; // size = Height of A
	int a_last_block_height = a_rows - (a_height_blocks * block_size);
	if (a_last_block_height > 0)
	{
		a_height_blocks++;
	}

	int b_width_blocks = b_cols / block_size; // size = Width of B
	int b_last_block_width = b_cols - (b_width_blocks * block_size);
	if (b_last_block_width > 0)
	{
		b_width_blocks++;
	}

	int commn_blocks = ab_commn / block_size; // size = Width of A or Height of B
	int commLastBlockWidth = ab_commn - (commn_blocks * block_size);
	if (commLastBlockWidth > 0)
	{
		commn_blocks++;
	}

	int a_block_height = block_size;
	int b_block_width;
	int comm_block_width;

	double k_tmp;
	int i_offset;
	for (int ib = 0; ib < a_height_blocks; ib++)
	{
		if (a_last_block_height > 0 && ib == (a_height_blocks - 1))
		{
			a_block_height = a_last_block_height;
		}
		b_block_width = comm_block_width = block_size;
		for (int jb = 0; jb < b_width_blocks; jb++)
		{
			if (b_last_block_width > 0 && jb == (b_width_blocks - 1))
			{
				b_block_width = b_last_block_width;
			}
			comm_block_width = block_size;
			for (int kb = 0; kb < commn_blocks; kb++)
			{
				if (commLastBlockWidth > 0 && kb == (commn_blocks - 1))
				{
					comm_block_width = commLastBlockWidth;
				}

				for (int i = ib * block_size; i < (ib * block_size) + a_block_height; i++)
				{
					i_offset = i * b_cols;
					for (int j = jb * block_size; j < (jb * block_size) + b_block_width;
					j++)
					{
						for (int k = kb * block_size;
						k < (kb * block_size) + comm_block_width; k++)
						{
							k_tmp = B[k * b_cols + j];
							if (A[i*ab_commn + k] != 0 && k_tmp != 0)
							{
								C[i_offset + j] += A[i*ab_commn + k] * k_tmp;
							}
						}
					}
				}
			}
		}
	}
}

void block_matrix_multiply_with_thread_offset(double* A, double* B, int a_rows, int b_cols, int ab_comn, int block_size, double* C, int thread_id) {

	int a_height_blocks = a_rows / block_size; // size = Height of A
	int a_last_block_height = a_rows - (a_height_blocks * block_size);
	if (a_last_block_height > 0) {
		a_height_blocks++;
	}

	int b_width_blocks = b_cols / block_size; // size = Width of B
	int b_last_block_width = b_cols - (b_width_blocks * block_size);
	if (b_last_block_width > 0) {
		b_width_blocks++;
	}

	int commn_blocks = ab_comn / block_size; // size = Width of A or Height of B
	int comm_last_block_width = ab_comn - (commn_blocks * block_size);
	if (comm_last_block_width > 0) {
		commn_blocks++;
	}

	int a_block_height = block_size;
	int b_block_width;
	int comm_block_width;

	int ib, jb, kb, i, j, k;
	int i_a_row_offset, k_b_row_offset, i_c_row_offset;
	for (ib = 0; ib < a_height_blocks; ib++) {
		if (a_last_block_height > 0 && ib == (a_height_blocks - 1)) {
			a_block_height = a_last_block_height;
		}
		b_block_width = block_size;
		for (jb = 0; jb < b_width_blocks; jb++) {
			if (b_last_block_width > 0 && jb == (b_width_blocks - 1)) {
				b_block_width = b_last_block_width;
			}
			comm_block_width = block_size;
			for (kb = 0; kb < commn_blocks; kb++) {
				if (comm_last_block_width > 0 && kb == (commn_blocks - 1)) {
					comm_block_width = comm_last_block_width;
				}

				for (i = ib * block_size; i < (ib * block_size) + a_block_height; i++) {
					i_a_row_offset = (i + thread_id)*ab_comn;
					i_c_row_offset = (i + thread_id)*b_cols;
					for (j = jb * block_size; j < (jb * block_size) + b_block_width;
					j++) {
						for (k = kb * block_size;
						k < (kb * block_size) + comm_block_width; k++) {
							k_b_row_offset = (k + thread_id)*b_cols;
							if (A[i_a_row_offset + k] != 0 && B[k_b_row_offset + j] != 0) {
								C[i_c_row_offset + j] += A[i_a_row_offset + k] * B[k_b_row_offset + j];
							}
						}
					}
				}
			}
		}
	}
}
