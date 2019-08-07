#include "HungarianOper.h"




void HungarianOper::Solve(Mat &cost, float max_cost)
{
	int rows = cost.size().height;//h y track
    int cols = cost.size().width;//w x detect
    Matrix<float> matrix(rows, cols);
    for (int row = 0; row < rows; row++) {
		float *pc = cost.ptr<float>(row);
        for (int col = 0; col < cols; col++) {
			float c = pc[col];
			c = (c > max_cost) ? (max_cost + 1e-5) : c;
            matrix(row, col) = c;
		//	printf("%dx%d: %f\n", col, row, matrix(row, col));
        }
    }
    //Munkres get matrix;
    Munkres<float> m;
    m.solve(matrix);
	
	for (int y = 0; y < rows; y++)
	{
		float *pc = cost.ptr<float>(y);
		for (int x = 0; x < cols; x++)
		{
			int tmp = (int)matrix(y, x);
			float c = pc[x];
			if ((tmp == 0) && (c < max_cost))
				pc[x] = max_cost;
			else
				pc[x] = 0;
		}	
	}
}










