
#pragma once

enum MatrixStringFormat
{
	MatrixStringFormatMathematica,
};

template <class D> struct SparseRowEntry
{
	SparseRowEntry() {}
	SparseRowEntry(UINT _col, D _val)
	{
		col = _col;
		val = _val;
	}
	UINT col;
	D val;
};

template <class D> struct SparseRow
{
	D& operator()(UINT col)
	{
		for(SparseRowEntry<D> &e : entries)
		{
			if(e.col == col) return e.val;
		}
		entries.pushBack(SparseRowEntry<D>(col, 0.0));
		return entries.back().val;
	}
	D operator()(UINT col) const
	{
		for(const SparseRowEntry<D> &e : entries)
		{
			if(e.col == col) return e.val;
		}
		return 0.0;
	}
	Vector< SparseRowEntry<D> > entries;
};

template <class D> class SparseMatrix
{
public:
	SparseMatrix()
	{
		m_rows = 0;
		m_cols = 0;
	}
	SparseMatrix(SparseMatrix &&s)
	{
		m_rows = s.m_rows;
		m_cols = s.m_cols;
		s.m_rows = 0;
		s.m_cols = 0;
		m_data = std::move(s.m_data);
	}
	explicit SparseMatrix(UINT squareDimension)
	{
		m_rows = squareDimension;
		m_cols = squareDimension;
		m_data.allocate(m_rows);
	}
	SparseMatrix(UINT rows, UINT cols)
	{
		m_rows = rows;
		m_cols = cols;
		m_data.allocate(m_rows);
	}
	SparseMatrix(const String &s, MatrixStringFormat format)
	{
		if(format == MatrixStringFormatMathematica)
		{
			//
			// this is really a dense format and should be loaded as such, then cast into a SparseMatrix
			//
			Vector<String> data = s.split("},{");
			m_rows = (UINT)data.size();
			m_cols = (UINT)data[0].split(",").size();
			m_data.allocate(m_rows);

			for(UINT row = 0; row < m_rows; row++)
			{
				Vector<String> values = data[row].split(",");
				for(UINT col = 0; col < values.size(); col++)
				{
					(*this)(row, col) = (D)values[col].findAndReplace("{","").findAndReplace("}","").toDouble();
				}
			}
		}
		else
		{
			MLIB_ERROR("invalid matrix string format");
		}
	}
	void operator=(SparseMatrix<D>&& V)
	{
		m_rows = s.m_rows;
		m_cols = s.m_cols;
		s.m_rows = 0;
		s.m_cols = 0;
		m_data = std::move(s.m_data);
	}

	//
	// Accessors
	//
	D& operator()(UINT row, UINT col)
	{
		return m_data[row](col);
	}
	D operator()(UINT row, UINT col) const
	{
		return m_data[row](col);
	}
	UINT rows() const
	{
		return m_rows;
	}
	UINT cols() const
	{
		return m_cols;
	}
	const SparseRow<D>& sparseRow(UINT row) const
	{
		return m_data[row];
	}
	const Vector<D> denseRow(UINT row) const
	{
		Vector<D> result(m_cols);
		for(UINT col = 0; col < m_cols; col++)
			result[col] = (*this)(row, col);
		return result;
	}
	const Vector<D> denseCol(UINT col) const
	{
		Vector<D> result(m_rows);
		for(UINT row = 0; row < m_rows; row++)
			result[row] = (*this)(row, col);
		return result;
	}
	Vector<D> diagonal() const
	{
		MLIB_ASSERT(square(), "diagonal called on non-square matrix");
		Vector<D> result(m_rows);
		for(UINT row = 0; row < m_rows; row++)
			result[row] = m_data[row](row);
		return result;
	}

	//
	// Math functions
	//
	SparseMatrix<D> transpose() const;
	D maxMagnitude() const;
	bool square() const
	{
		return (m_rows == m_cols);
	}

	//
	// overloaded operator helpers
	//
	static SparseMatrix<D> add(const SparseMatrix<D> &A, const SparseMatrix<D> &B);
	static SparseMatrix<D> subtract(const SparseMatrix<D> &A, const SparseMatrix<D> &B);
	static SparseMatrix<D> multiply(const SparseMatrix<D> &A, D c);
	static Vector<D> multiply(const SparseMatrix<D> &A, const Vector<D> &v);
	static SparseMatrix<D> multiply(const SparseMatrix<D> &A, const SparseMatrix<D> &B);
	
	// returns the scalar v^T A v
	static D quadratic(const SparseMatrix<D> &A, const Vector<D> &v)
	{
		return Vector<D>::dotProduct(v, multiply(A, v));
	}

private:
	UINT m_rows, m_cols;
    Vector< SparseRow<D> > m_data;

	// set is a more efficient version of operator() that assumes the entry
	// does not exist.
	void insert(UINT row, UINT col, double val)
	{
		m_data[row].entries.pushBack(SparseRowEntry<D>(col, val));
	}
};

template<class D>
SparseMatrix<D> operator + (const SparseMatrix<D> &A, const SparseMatrix<D> &B)
{
	return SparseMatrix<D>::add(A, B);
}

template<class D>
SparseMatrix<D> operator - (const SparseMatrix<D> &A, const SparseMatrix<D> &B)
{
	return SparseMatrix<D>::subtract(A, B);
}

template<class D>
SparseMatrix<D> operator * (const SparseMatrix<D> &A, const SparseMatrix<D> &B)
{
	return SparseMatrix<D>::multiply(A, B);
}

template<class D>
Vector<D> operator * (const SparseMatrix<D> &A, const Vector<D> &B)
{
	return SparseMatrix<D>::multiply(A, B);
}

template<class D>
SparseMatrix<D> operator * (const SparseMatrix<D> &A, D val)
{
	return SparseMatrix<D>::multiply(A, val);
}

//typedef SparseMatrix<float> SparseMatrixf;
//typedef SparseMatrix<double> SparseMatrixd;

#include "sparseMatrix.cpp"