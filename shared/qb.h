#pragma once

#include <stdio.h>
#include <stdlib.h>


class QuickBuffer
{
private:
	char* m_buffer;
	size_t m_maxSize;
	size_t m_cursor;

public:
	QuickBuffer(size_t maxSize = 0x1000)
	{
		m_buffer = (char*)malloc(maxSize);
		m_maxSize = maxSize;
		m_cursor = 0;
	}

	~QuickBuffer()
	{
		free(m_buffer);
	}

	template<typename T>
	void Write(const T& value)
	{
		memcpy(m_buffer + m_cursor, &value, sizeof(T));
		m_cursor += sizeof(T);
	}

	void Reset()
	{
		m_cursor = 0;
	}

	const void* GetBuffer() { return m_buffer; }
	size_t GetSize() { return m_cursor; }
};