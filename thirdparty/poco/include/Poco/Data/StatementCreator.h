//
// StatementCreator.h
//
// Library: Data
// Package: DataCore
// Module:  StatementCreator
//
// Definition of the StatementCreator class.
//
// Copyright (c) 2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#ifndef Data_StatementCreator_INCLUDED
#define Data_StatementCreator_INCLUDED


#include "Poco/Data/Data.h"
#include "Poco/Data/SessionImpl.h"
#include "Poco/Data/Statement.h"
#include "Poco/AutoPtr.h"


namespace Poco {
namespace Data {


class Data_API StatementCreator
	/// A StatementCreator creates Statements.
{
public:
	StatementCreator();
		/// Creates an unitialized StatementCreator.

	StatementCreator(Poco::AutoPtr<SessionImpl> ptrImpl);
		/// Creates a StatementCreator.

	StatementCreator(const StatementCreator& other);
		/// Creates a StatementCreator by copying another one.

	StatementCreator(StatementCreator&& other) noexcept;
		/// Creates a StatementCreator by moving another one.

	~StatementCreator();
		/// Destroys the StatementCreator.

	StatementCreator& operator = (const StatementCreator& other);
		/// Assignment operator.

	StatementCreator& operator = (StatementCreator&& other) noexcept;
		/// Assignment operator.

	void swap(StatementCreator& other) noexcept;
		/// Swaps the StatementCreator with another one.

	template <typename T>
	Statement operator << (const T& t)
		/// Creates a Statement.
	{
		if (!_ptrImpl->isConnected())
			throw NotConnectedException(_ptrImpl->connectionString());

		Statement stmt(_ptrImpl->createStatementImpl());
		stmt << t;
		return stmt;
	}

	void reset();
		/// Resets the StatementCreator.

private:
	Poco::AutoPtr<SessionImpl> _ptrImpl;
};

//
// inlines
//


inline void StatementCreator::swap(StatementCreator& other) noexcept
{
	using std::swap;
	swap(_ptrImpl, other._ptrImpl);
}


inline void StatementCreator::reset()
{
	_ptrImpl = nullptr;
}


} } // namespace Poco::Data


#endif // Data_StatementCreator_INCLUDED
