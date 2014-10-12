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

namespace	cl3
{
	namespace	system
	{
		namespace	compiler
		{
			namespace	jit
			{
				class	TModule;
				class	TFunction;
				class	TBlock;
				class	IInstruction;
				class	ITerminationInstruction;

				//	IInstruction & ITerminationInstruction:
				class	TReturnInstruction;
				class	TReturnInstruction;
				class	TConditionalJumpInstruction;
				class	TJumpInstruction;
				class	TSwitchInstruction;
				class	TIndirectJumpInstruction;
				class	TResumeInstruction;
				class	TUnreachableInstruction;

				//	IInstruction:
				class	TAddInstruction;
				class	TSubInstruction;
				class	TMulInstruction;
				class	TDivInstruction;
				class	TModInstruction;
				class	TShiftInstruction;
				class	TAndInstruction;
				class	TOrInstruction;
				class	TXorInstruction;
				class	TAllocaInstruction;
				class	TLoadInstruction;
				class	TStoreInstruction;
				class	TCompXInstruction;
				class	TAtomicRMWInstruction;
				class	TConvertInstruction;
				class	TBitCastInstruction;
				class	TCompareInstruction;
				class	TSelectInstruction;
				class	TCallInstruction;

				class	IValue;
				class	ILiteral;
				template<class T> class TLiteral;
				class	IVariable;
				template<class T> class TTypedVariable;
				class	TTemporary;
				class	IProperty;
				template<class T> class TProperty;

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

				class	IInstruction
				{
					public:
						virtual	CLASS	~IInstruction	();
				};

				class	ITerminationInstruction : public IInstruction
				{
				};

				class	IValue
				{
					private:
						CLASS IValue(const IValue&) = delete;
						IValue& operator=(const IValue&) = delete;

					protected:
						const types::typeinfo::TRTTI* rtti;

						CL3PUBF	CLASS	IValue		(const types::typeinfo::TRTTI*);

					public:
						CL3PUBF	const types::typeinfo::TRTTI*
										Datatype	() const CL3_GETTER;
						CL3PUBF	void	Datatype	(const types::typeinfo::TRTTI*) CL3_SETTER;

						virtual	CLASS	~IValue	();
				};

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
				};

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
				};

				class	TTemporary : public IValue
				{
					friend class TFunction;
					private:
						CL3PUBF	CLASS	TTemporary	(TFunction* function, IValue* source);

					protected:
						TFunction* function;
						IValue* source;

					public:
						inline	TFunction&	Function	() const CL3_GETTER { return *this->function; }
						inline	IValue&		Source		() const CL3_GETTER { return *this->source; }
						inline	void		Source		(IValue& source_new) CL3_SETTER { source = &source_new; }
				};

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

						CL3PUBF	TAddInstruction&		AddAdd			(IValue& op1, IValue& op2);
						CL3PUBF	TSubInstruction&		AddSub			(IValue& op1, IValue& op2);
						CL3PUBF	TMulInstruction&		AddMul			(IValue& op1, IValue& op2);
						CL3PUBF	TDivInstruction&		AddDiv			(IValue& op1, IValue& op2);
						CL3PUBF	TModInstruction&		AddMod			(IValue& op1, IValue& op2);
						CL3PUBF	TShiftInstruction&		AddShift		(IValue& value, IValue& times, EDirection direction);
						CL3PUBF	TAndInstruction&		AddAnd			(IValue& op1, IValue& op2);
						CL3PUBF	TOrInstruction&			AddOr			(IValue& op1, IValue& op2);
						CL3PUBF	TXorInstruction&		AddXor			(IValue& op1, IValue& op2);
						CL3PUBF	TAllocaInstruction&		AddAlloca		(const types::typeinfo::TRTTI* datatype, IValue& count);
						CL3PUBF	TLoadInstruction&		AddLoad			(IValue& pointer);
						CL3PUBF	TStoreInstruction&		AddStore		(IValue& pointer, IValue& value);
						CL3PUBF	TCompXInstruction&		AddCompX		(IValue& pointer, IValue& compare, IValue& newv);
						CL3PUBF	TAtomicRMWInstruction&	AddAtomicRMW	(EAtomicOperation op, IValue& pointer, IValue& value);
						CL3PUBF	TConvertInstruction&	AddConvert		(IValue& value, const types::typeinfo::TRTTI* datatype);
						CL3PUBF	TBitCastInstruction&	AddBitCast		(IValue& value, const types::typeinfo::TRTTI* datatype);
						CL3PUBF	TCompareInstruction&	AddCompare		(ECompare compare, IValue& v1, IValue& v2);
						CL3PUBF	TSelectInstruction&		AddSelect		(IValue& cond, IValue& value_true, IValue& value_false);
						CL3PUBF	TCallInstruction&		AddCall			(TFunction& func);	//	add parameters

						inline	TFunction&				Function		() const CL3_GETTER { return *this->function; }
				};

				class	TFunction : public TBlock
				{
					friend class TModule;
					private:
						TFunction& operator=(const TFunction&) = delete;
						CLASS TFunction(const TFunction&) = delete;

						CL3PUBF	CLASS	TFunction	(TModule* module);
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
								CL3PUBF	IValue*	DefaultValue	() const CL3_GETTER;
						};

					protected:
						TModule* module;
						io::collection::list::TList<TParameter*> parameters;
						io::collection::list::TList<TTemporary*> temporaries;
						io::collection::list::TList<TBlock*> blocks;

					public:
						CL3PUBF	CLASS	~TFunction	();

						CL3PUBF	TParameter&	AddParameter	(const types::typeinfo::TRTTI* datatype, IValue* default_value = NULL);
						CL3PUBF	TTemporary&	AddTemporary	(IValue& source);
						CL3PUBF	TBlock&		AddBlock		();

						inline	TModule&	Module			() const CL3_GETTER { return *this->module; }
				};

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

						CL3PUBF	TFunction&	AddFunction	(const types::typeinfo::TRTTI* return_type);
						CL3PUBF	IVariable&	AddVariable	(const types::typeinfo::TRTTI* datatype, const void* value_init);
						CL3PUBF	ILiteral&	AddLiteral	(const types::typeinfo::TRTTI* datatype, const void* value);
				};
			}
		}
	}
}

#endif
