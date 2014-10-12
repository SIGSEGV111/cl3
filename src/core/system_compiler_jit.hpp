/*
    libcl3 - common library version 3
    Copyright (C) 2013	Simon Brennecke

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef	_include_cl3_core_system_compiler_jit_hpp_
#define	_include_cl3_core_system_compiler_jit_hpp_

#include "io_collection_list.hpp"
#include "system_types_typeinfo.hpp"
#include "system_memory.hpp"

namespace	cl3
{
	namespace	system
	{
		namespace	compiler
		{
			namespace	jit
			{
				class	CL3PUBT	TModule;
				class	CL3PUBT	TFunction;
				class	CL3PUBT	TBlock;
				class	CL3PUBT	IInstruction;
				class	CL3PUBT	ITerminationInstruction;
				class	CL3PUBT	TBinaryOpInstruction;

				/*************************************************************************************/

				//	IInstruction & ITerminationInstruction:
				class	CL3PUBT	TReturnInstruction;
				class	CL3PUBT	TConditionalJumpInstruction;
				class	CL3PUBT	TJumpInstruction;
				class	CL3PUBT	TSwitchInstruction;
				class	CL3PUBT	TIndirectJumpInstruction;
				class	CL3PUBT	TUnreachableInstruction;

				//	IInstruction:
				class	CL3PUBT	TBinaryOpInstruction;
				class	CL3PUBT	TShiftInstruction;
				class	CL3PUBT	TAllocaInstruction;
				class	CL3PUBT	TLoadInstruction;
				class	CL3PUBT	TStoreInstruction;
				class	CL3PUBT	TAtomicSwapInstruction;
				class	CL3PUBT	TAtomicRMWInstruction;
				class	CL3PUBT	TConvertInstruction;
				class	CL3PUBT	TBitCastInstruction;
				class	CL3PUBT	TCompareInstruction;
				class	CL3PUBT	TSelectInstruction;
				class	CL3PUBT	TCallInstruction;

				/*************************************************************************************/

				class	CL3PUBT	IValue;
				class	CL3PUBT	ILiteral;
				template<class T> class CL3PUBT	TLiteral;
				class	CL3PUBT	IVariable;
				template<class T> class CL3PUBT	TTypedVariable;
// 				class	CL3PUBT	TTemporary;
				class	CL3PUBT	IProperty;
				template<class T> class CL3PUBT	TProperty;

				/*************************************************************************************/

				enum	EDirection
				{
					DIRECTION_LEFT,
					DIRECTION_RIGHT
				};

				enum	EAtomicOperation
				{
					ATOMIC_OPERATION_EXCHANGE,
					ATOMIC_OPERATION_ADD,
					ATOMIC_OPERATION_SUB,
					ATOMIC_OPERATION_AND,
					ATOMIC_OPERATION_NAND,
					ATOMIC_OPERATION_OR,
					ATOMIC_OPERATION_XOR,
					ATOMIC_OPERATION_SMAX,
					ATOMIC_OPERATION_SMIN,
					ATOMIC_OPERATION_UMAX,
					ATOMIC_OPERATION_UMIN
				};

				enum	ECompare
				{
					COMPARE_EQUAL,
					COMPARE_NOTEQUAL,
					COMPARE_UNSIGNED_GREATERTHAN,
					COMPARE_UNSIGNED_GREATEROREQUAL,
					COMPARE_UNSIGNED_LESSTHAN,
					COMPARE_UNSIGNED_LESSOREQUAL,
					COMPARE_SIGNED_GREATERTHAN,
					COMPARE_SIGNED_GREATEROREQUAL,
					COMPARE_SIGNED_LESSTHAN,
					COMPARE_SIGNED_LESSOREQUAL
				};

				enum	EBinaryOperation
				{
					BINARY_OPERATION_ADD,
					BINARY_OPERATION_SUB,
					BINARY_OPERATION_MUL,
					BINARY_OPERATION_DIV,
					BINARY_OPERATION_MOD,
					BINARY_OPERATION_AND,
					BINARY_OPERATION_OR,
					BINARY_OPERATION_XOR
				};

				/*************************************************************************************/

				class	IValue
				{
					private:
						CLASS IValue(const IValue&) = delete;
						IValue& operator=(const IValue&) = delete;

					protected:
						const types::typeinfo::TRTTI* datatype;

						CL3PUBF	CLASS	IValue		(const types::typeinfo::TRTTI* datatype);

					public:
						inline	const types::typeinfo::TRTTI*
										Datatype	() const CL3_GETTER { return this->datatype; }
						inline	void	Datatype	(const types::typeinfo::TRTTI* datatype) CL3_SETTER { this->datatype = datatype; }

						virtual	IValue&	CopyTo	(TBlock*) const = 0;
						CL3PUBF	virtual	CLASS	~IValue	();
				};

				/*************************************************************************************/

				class	ILiteral : public IValue
				{
					template<class T> friend class TTypedLiteral;
					friend class TGenericLiteral;
					private:
						CL3PUBF	CLASS	ILiteral	(TModule*, const types::typeinfo::TRTTI*);

					protected:
						TModule* module;

					public:
						inline	TModule&	Module			() const CL3_GETTER { return *this->module; }
						virtual	const void*	GenericValue	() const CL3_GETTER = 0;
						virtual	void*		GenericValue	() CL3_GETTER = 0;
				};

				template<class T>
				class	TTypedLiteral : public ILiteral
				{
					friend class TModule;
					private:
						inline	CLASS		TTypedLiteral	(TModule* module, const T& value_init) : ILiteral(module, &types::typeinfo::TCTTI<T>::rtti), value(value_init) {}

					protected:
						T value;

					public:
						inline	const void*	GenericValue	() const final override CL3_GETTER { return &this->value; }
						inline	void*		GenericValue	() final override CL3_GETTER { return &this->value; }

						inline	const T&	TypedValue		() const CL3_GETTER { return this->value; }
						inline	T&			TypedValue		() CL3_GETTER { return this->value; }
						inline	void		TypedValue		(const T& value_new) CL3_SETTER { this->value = value_new; }

						inline	TTypedLiteral&	CopyTo	(TBlock* block_new) const final override;
				};

				class	TGenericLiteral : public ILiteral
				{
					friend class TModule;
					private:
						CL3PUBF	CLASS		TGenericLiteral	(TModule* module, const types::typeinfo::TRTTI* datatype, const void* value_init);

					protected:
						void* value;

					public:
						inline	const void*	GenericValue	() const final override CL3_GETTER { return this->value; }
						inline	void*		GenericValue	() final override CL3_GETTER { return this->value; }

						CL3PUBF	TGenericLiteral&	CopyTo	(TBlock* block_new) const final override;
				};

				/*************************************************************************************/

				class	IVariable : public IValue
				{
					template<class T> friend class TTypedVariable;
					friend class TGenericVariable;
					private:
						CL3PUBF	CLASS	IVariable	(TModule*, const types::typeinfo::TRTTI*);

					protected:
						TModule* module;

					public:
						inline	TModule&	Module			() const CL3_GETTER { return *this->module; }
						virtual	const void*	GenericValue	() const CL3_GETTER = 0;
						virtual	void*		GenericValue	() CL3_GETTER = 0;
				};

				template<class T>
				class	TTypedVariable : public IVariable
				{
					friend class TModule;
					private:
						inline	CLASS		TTypedVariable	(TModule* module, const T& value_init) : IVariable(module, &types::typeinfo::TCTTI<T>::rtti), value(value_init) {}

					protected:
						T value;

					public:
						inline	const void*	GenericValue	() const final override CL3_GETTER { return &this->value; }
						inline	void*		GenericValue	() final override CL3_GETTER { return &this->value; }

						inline	const T&	TypedValue		() const CL3_GETTER { return this->value; }
						inline	T&			TypedValue		() CL3_GETTER { return this->value; }
						inline	void		TypedValue		(const T& value_new) CL3_SETTER { this->value = value_new; }

						inline	TTypedVariable&	CopyTo	(TBlock* block_new) const final override;
				};

				class	TGenericVariable : public IVariable
				{
					friend class TModule;
					private:
						CL3PUBF	CLASS		TGenericVariable(TModule* module, const types::typeinfo::TRTTI* datatype, const void* value_init);

					protected:
						void* value;

					public:
						inline	const void*	GenericValue	() const final override CL3_GETTER { return this->value; }
						inline	void*		GenericValue	() final override CL3_GETTER { return this->value; }

						CL3PUBF	TGenericVariable&	CopyTo	(TBlock* block_new) const final override;
				};

				/*************************************************************************************/

// 				class	TTemporary : public IValue
// 				{
// 					friend class TFunction;
// 					private:
// 						CL3PUBF	CLASS	TTemporary	(TFunction* function, IValue* source);
//
// 					protected:
// 						TFunction* function;
// 						IValue* source;
//
// 					public:
// 						inline	TFunction&	Function	() const CL3_GETTER { return *this->function; }
// 						inline	IValue&		Source		() const CL3_GETTER { return *this->source; }
// 						inline	void		Source		(IValue& source_new) CL3_SETTER { source = &source_new; }
//
// 						CL3PUBF	TTemporary&	CopyTo	(TBlock* block_new) const final override;
// 				};

				/*************************************************************************************/

				class	IInstruction
				{
					friend class TBinaryOpInstruction;
					friend class ITerminationInstruction;
					private:
						CLASS IInstruction(const IInstruction&) = delete;
						IInstruction& operator=(const IInstruction&) = delete;

						CL3PUBF	CLASS	IInstruction	(TBlock* block);

					protected:
						TBlock* block;

					public:
						virtual	CLASS	~IInstruction	();
						virtual	IInstruction&	CopyTo	(TBlock*) const = 0;

						inline	TBlock&	Block	() const CL3_GETTER { return *this->block; }
				};

				class	ITerminationInstruction : public IInstruction
				{
					friend class TReturnInstruction;
					private:
						inline	CLASS	ITerminationInstruction	(TBlock* block) : IInstruction(block) {}
				};

				class	TReturnInstruction : public ITerminationInstruction
				{
					friend class TBlock;
					private:
						inline	CLASS	TReturnInstruction	(TBlock* block, IValue* value = NULL) : ITerminationInstruction(block), value(value) {}

					protected:
						IValue* value;

					public:
						inline	IValue*	Value	() const CL3_GETTER { return this->value; }
						inline	void	Value	(IValue* value_new) CL3_SETTER { this->value = value_new; }

						CL3PUBF	TReturnInstruction&	CopyTo	(TBlock*) const;
				};

				class	TBinaryOpInstruction : public IInstruction, public IValue
				{
					friend class TBlock;
					private:
						CL3PUBF	CLASS	TBinaryOpInstruction	(TBlock* block, EBinaryOperation op, IValue* lhs, IValue* rhs);

					protected:
						EBinaryOperation op;
						IValue* lhs;
						IValue* rhs;

					public:
						inline	EBinaryOperation Operation	() const CL3_GETTER { return this->op; }
						inline	IValue& LHS	() const CL3_GETTER { return *this->lhs; }
						inline	IValue& RHS	() const CL3_GETTER { return *this->rhs; }

						inline	void	Operation	(EBinaryOperation op) CL3_SETTER { this->op = op; }
						inline	void	LHS			(IValue& lhs) CL3_SETTER { this->lhs = &lhs; }
						inline	void	RHS			(IValue& rhs) CL3_SETTER { this->rhs = &rhs; }

						CL3PUBF	TBinaryOpInstruction&	CopyTo	(TBlock*) const;
				};

				/*************************************************************************************/

				class	TBlock
				{
					friend class TFunction;
					private:
						TBlock& operator=(const TBlock&) = delete;
						CLASS TBlock(const TBlock&) = delete;

						CL3PUBF	CLASS	TBlock	(TFunction* function);
						CL3PUBF	CLASS	TBlock	(TFunction* function, const TBlock&);

					protected:
						TFunction* function;
						io::collection::list::TList<IInstruction*> instructions;

					public:
						CL3PUBF	CLASS	~TBlock	();

						class	TTermination
						{
							friend class TBlock;
							private:
								TTermination& operator=(const TTermination&) = delete;
								CLASS TTermination(const TTermination&) = delete;

								CL3PUBF	CLASS	TTermination	(TBlock* block);

							protected:
								TBlock* block;
								memory::TUniquePtr<ITerminationInstruction> instruction;

							public:
								CL3PUBF	TReturnInstruction&				SetReturn			();
								CL3PUBF	TReturnInstruction&				SetReturn			(IValue& value);
								CL3PUBF	TConditionalJumpInstruction&	SetConditionalJump	(IValue& cond, TBlock& block_true, TBlock& block_false);
								CL3PUBF	TJumpInstruction&				SetJump				(TBlock& block);
								CL3PUBF	TSwitchInstruction&				SetSwitch			(IValue& value, TBlock& block_default);	//	add cases
								CL3PUBF	TIndirectJumpInstruction&		SetIndirectJump		(IValue& block_address);	//	must add all possible targets
								CL3PUBF	TUnreachableInstruction&		SetUnreachable		();
						} termination;

						CL3PUBF	TBinaryOpInstruction&	AddBinOp		(EBinaryOperation op, IValue& lhs, IValue& rhs);
						inline	TBinaryOpInstruction&	AddAdd			(IValue& lhs, IValue& rhs) { return AddBinOp(BINARY_OPERATION_ADD, lhs, rhs); }
						inline	TBinaryOpInstruction&	AddSub			(IValue& lhs, IValue& rhs) { return AddBinOp(BINARY_OPERATION_SUB, lhs, rhs); }
						inline	TBinaryOpInstruction&	AddMul			(IValue& lhs, IValue& rhs) { return AddBinOp(BINARY_OPERATION_MUL, lhs, rhs); }
						inline	TBinaryOpInstruction&	AddDiv			(IValue& lhs, IValue& rhs) { return AddBinOp(BINARY_OPERATION_DIV, lhs, rhs); }
						inline	TBinaryOpInstruction&	AddMod			(IValue& lhs, IValue& rhs) { return AddBinOp(BINARY_OPERATION_MOD, lhs, rhs); }
						inline	TBinaryOpInstruction&	AddAnd			(IValue& lhs, IValue& rhs) { return AddBinOp(BINARY_OPERATION_AND, lhs, rhs); }
						inline	TBinaryOpInstruction&	AddOr			(IValue& lhs, IValue& rhs) { return AddBinOp(BINARY_OPERATION_OR , lhs, rhs); }
						inline	TBinaryOpInstruction&	AddXor			(IValue& lhs, IValue& rhs) { return AddBinOp(BINARY_OPERATION_XOR, lhs, rhs); }
						CL3PUBF	TShiftInstruction&		AddShift		(IValue& value, IValue& times, EDirection direction);
						CL3PUBF	TAllocaInstruction&		AddAlloca		(const types::typeinfo::TRTTI* datatype, IValue& count);
						CL3PUBF	TLoadInstruction&		AddLoad			(IValue& pointer);
						CL3PUBF	TStoreInstruction&		AddStore		(IValue& pointer, IValue& value);
						CL3PUBF	TAtomicSwapInstruction&	AddAtomicSwap	(IValue& pointer, IValue& compare, IValue& newv);
						CL3PUBF	TAtomicRMWInstruction&	AddAtomicRMW	(EAtomicOperation op, IValue& pointer, IValue& value);
						CL3PUBF	TConvertInstruction&	AddConvert		(IValue& value, const types::typeinfo::TRTTI* datatype);
						CL3PUBF	TBitCastInstruction&	AddBitCast		(IValue& value, const types::typeinfo::TRTTI* datatype);
						CL3PUBF	TCompareInstruction&	AddCompare		(ECompare compare, IValue& v1, IValue& v2);
						CL3PUBF	TSelectInstruction&		AddSelect		(IValue& cond, IValue& value_true, IValue& value_false);
						CL3PUBF	TCallInstruction&		AddCall			(TFunction& func);	//	add parameters

						inline	TFunction&				Function		() const CL3_GETTER { return *this->function; }
				};

				/*************************************************************************************/

				class	TFunction : public TBlock
				{
					friend class TModule;
					private:
						TFunction& operator=(const TFunction&) = delete;
						CLASS TFunction(const TFunction&) = delete;

						CL3PUBF	CLASS	TFunction	(TModule* module, const types::typeinfo::TRTTI* return_type);
						CL3PUBF	CLASS	TFunction	(TModule* module, const TFunction&);

					public:
						class	TParameter : public IValue
						{
							friend class TFunction;
							private:
								CL3PUBF	CLASS	TParameter	(TFunction* function, const types::typeinfo::TRTTI* datatype, IValue* default_value);

							protected:
								TFunction* function;
								IValue* default_value;

							public:
								CL3PUBF	IValue*		DefaultValue	() const CL3_GETTER;
								CL3PUBF	TParameter&	CopyTo			(TBlock* block_new) const final override;
						};

					protected:
						TModule* module;
						const types::typeinfo::TRTTI* return_type;
						io::collection::list::TList<TParameter*> parameters;
// 						io::collection::list::TList<TTemporary*> temporaries;
						io::collection::list::TList<TBlock*> blocks;

					public:
						CL3PUBF	CLASS	~TFunction	();

						CL3PUBF	TParameter&	AddParameter	(const types::typeinfo::TRTTI* datatype, IValue* default_value = NULL);
// 						CL3PUBF	TTemporary&	AddTemporary	(IValue& source);
						CL3PUBF	TBlock&		AddBlock		();

						inline	TModule&	Module			() const CL3_GETTER { return *this->module; }
				};

				/*************************************************************************************/

				class	TModule
				{
					private:
						TModule& operator=(const TModule&) = delete;

					protected:
						io::collection::list::TList<TFunction*> functions;
						io::collection::list::TList<IVariable*> variables;
						io::collection::list::TList<ILiteral*> literals;

					public:
						CL3PUBF	CLASS	TModule		();
						CL3PUBF	CLASS	TModule		(const TModule&);
						CL3PUBF	CLASS	~TModule	();

						CL3PUBF	TFunction&			AddFunction	(const types::typeinfo::TRTTI* return_type);
						CL3PUBF	TGenericVariable&	AddVariable	(const types::typeinfo::TRTTI* datatype, const void* value_init);
						CL3PUBF	TGenericLiteral&	AddLiteral	(const types::typeinfo::TRTTI* datatype, const void* value);

						template<class T>
						inline	TTypedLiteral<T>&	AddLiteral	(const T& value)
						{
							memory::TUniquePtr< TTypedLiteral<T> > p = memory::MakeUniquePtr(new TTypedLiteral<T>(this, value));
							literals.Add(p.Object());
							return *p.Claim();
						}
				};

				/*************************************************************************************/

				template<class T>
				TTypedLiteral<T>&	TTypedLiteral<T>::CopyTo	(TBlock* block_new) const
				{
					return  block_new->Function().Module().AddLiteral(value);	//	TODO: use template variant of AddLiteral
				}

				template<class T>
				TTypedVariable<T>&	TTypedVariable<T>::CopyTo	(TBlock* block_new) const
				{
					return  block_new->Function().Module().AddVariable(value);	//	TODO: use template variant of AddLiteral
				}
			}
		}
	}
}

#endif
