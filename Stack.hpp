#pragma once

template <class T>
class Stack
{
private:
	size_t m_capacity;

	T* m_data;

	int m_tos;

	void resize();

public:
	Stack();

	Stack(const Stack& rhs) = delete;
	Stack& operator=(const Stack& rhs) = delete;

	~Stack();

	bool is_full() const;

	bool is_empty() const;

	void push(const T& element);

	T pop();

	T top() const;

	size_t size() const;
};

template<class T>
inline void Stack<T>::resize()
{
	T* data = new T[m_capacity * 2];

	for (size_t i = 0; i < m_tos; i++)
	{
		data[i] = m_data[i];
	}

	m_capacity *= 2;

	delete[] m_data;
	m_data = data;
}

template<class T>
inline Stack<T>::Stack()
	: m_capacity(8),
	m_data(new T[m_capacity]),
	m_tos(0)
{ }

template<class T>
inline Stack<T>::~Stack()
{
	delete[] m_data;
}

template<class T>
inline bool Stack<T>::is_full() const
{
	return m_tos == m_capacity;
}

template<class T>
inline bool Stack<T>::is_empty() const
{
	return m_tos == 0;
}

template<class T>
inline void Stack<T>::push(const T& element)
{
	if (is_full())
	{
		resize();
	}

	m_data[m_tos++] = element;
}

template<class T>
inline T Stack<T>::pop()
{
	return m_data[--m_tos];
}

template<class T>
inline T Stack<T>::top() const
{
	return m_data[m_tos - 1];
}

template<class T>
inline size_t Stack<T>::size() const
{
	return m_tos;
}
