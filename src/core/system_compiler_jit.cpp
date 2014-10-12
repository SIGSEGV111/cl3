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

#ifndef INSIDE_CL3
#error "compiling cl3 source code but macro INSIDE_CL3 is not defined"
#endif

#include "system_compiler_jit.hpp"

extern "C" void* memcpy(void* dest, const void* src, size_t n);

namespace	cl3
{
	namespace	system
	{
		namespace	compiler
		{
			namespace	jit
			{
				using namespace memory;

				template<class TL, class TP>
				static	TP&	AddToListAndReturnValue	(io::collection::list::TList<TL*>& list, TP* ptr)
				{
					TUniquePtr<TP> p = MakeUniquePtr(ptr);
					list.Add(p.Object());
					return *p.Claim();
				}

				/*************************************************************************************/

				CLASS	IInstruction::IInstruction	(TBlock* block) : block(block)
				{
				}

				CLASS	IInstruction::~IInstruction	()
				{
				}

				CLASS	TBinaryOpInstruction::TBinaryOpInstruction	(TBlock* block, EBinaryOperation op, IValue* lhs, IValue* rhs) : IInstruction(block), op(op), lhs(lhs), rhs(rhs)
				{
				}

				TBinaryOpInstruction&	TBinaryOpInstruction::CopyTo	(TBlock* block_new) const
				{
					//return block_new->AddBinOp(this->op, this->lhs->CopyTo(block_new), this->rhs->CopyTo(block_new));
					CL3_NOT_IMPLEMENTED;	//	TODO: requires "module.Lookup(value)"
				}

				/*************************************************************************************/

				CLASS	IValue::IValue		(const types::typeinfo::TRTTI* datatype) : datatype(datatype)
				{
				}

				CLASS	IValue::~IValue		()
				{
				}

				/*************************************************************************************/

				CLASS	ILiteral::ILiteral	(TModule* module, const types::typeinfo::TRTTI* datatype) : IValue(datatype), module(module)
				{
				}

				CLASS	TGenericLiteral::TGenericLiteral	(TModule* module, const types::typeinfo::TRTTI* datatype, const void* value_init) : ILiteral(module, datatype)
				{
					this->value = Alloc(1, datatype);
					if(value_init)
						::memcpy(this->value, value_init, datatype->sz_bytes);
				}

				TGenericLiteral&	TGenericLiteral::CopyTo	(TBlock* block_new) const
				{
					return block_new->Function().Module().AddLiteral(datatype, value);
				}

				/*************************************************************************************/

				CLASS	IVariable::IVariable	(TModule* module, const types::typeinfo::TRTTI* datatype) : IValue(datatype), module(module)
				{
				}

				CLASS	TGenericVariable::TGenericVariable	(TModule* module, const types::typeinfo::TRTTI* datatype, const void* value_init) : IVariable(module, datatype)
				{
					this->value = Alloc(1, datatype);
					if(value_init)
						::memcpy(this->value, value_init, datatype->sz_bytes);
				}

				TGenericVariable&	TGenericVariable::CopyTo	(TBlock* block_new) const
				{
					return block_new->Function().Module().AddVariable(datatype, value);
				}

				/*************************************************************************************/

				CLASS	TTemporary::TTemporary	(TFunction* function, IValue* source) : IValue(source->Datatype()), function(function), source(source)
				{
				}

				TTemporary&	TTemporary::CopyTo	(TBlock* block_new) const
				{
					return block_new->Function().AddTemporary(source->CopyTo(block_new));
				}

				/*************************************************************************************/

				CLASS	TBlock::TBlock	(TFunction* function) : function(function), termination(this)
				{
				}

				CLASS	TBlock::TBlock	(TFunction* function, const TBlock& other) : function(function), termination(this)
				{
				}

				CLASS	TBlock::~TBlock	()
				{
					const usys_t n = instructions.Count();
					for(usys_t i = 0; i < n; i++)
						delete instructions[i];
				}

				TBinaryOpInstruction&	TBlock::AddBinOp		(EBinaryOperation op, IValue& lhs, IValue& rhs)
				{
					return AddToListAndReturnValue(instructions, new TBinaryOpInstruction(this, op, &lhs, &rhs));
				}

				TShiftInstruction&		TBlock::AddShift		(IValue& value, IValue& times, EDirection direction)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TAllocaInstruction&		TBlock::AddAlloca		(const types::typeinfo::TRTTI* datatype, IValue& count)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TLoadInstruction&		TBlock::AddLoad			(IValue& pointer)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TStoreInstruction&		TBlock::AddStore		(IValue& pointer, IValue& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TCompXInstruction&		TBlock::AddCompX		(IValue& pointer, IValue& compare, IValue& newv)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TAtomicRMWInstruction&	TBlock::AddAtomicRMW	(EAtomicOperation op, IValue& pointer, IValue& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TConvertInstruction&	TBlock::AddConvert		(IValue& value, const types::typeinfo::TRTTI* datatype)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TBitCastInstruction&	TBlock::AddBitCast		(IValue& value, const types::typeinfo::TRTTI* datatype)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TCompareInstruction&	TBlock::AddCompare		(ECompare compare, IValue& v1, IValue& v2)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TSelectInstruction&		TBlock::AddSelect		(IValue& cond, IValue& value_true, IValue& value_false)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TCallInstruction&		TBlock::AddCall			(TFunction& func)
				{
					CL3_NOT_IMPLEMENTED;
				}

				/*************************************************************************************/

				CLASS							TBlock::TTermination::TTermination			(TBlock* block) : block(block)
				{
				}

				TReturnInstruction&				TBlock::TTermination::SetReturn				()
				{
					CL3_NOT_IMPLEMENTED;
				}

				TReturnInstruction&				TBlock::TTermination::SetReturn				(IValue& value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TConditionalJumpInstruction&	TBlock::TTermination::SetConditionalJump	(IValue& cond, TBlock& block_true, TBlock& block_false)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TJumpInstruction&				TBlock::TTermination::SetJump				(TBlock& block)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TSwitchInstruction&				TBlock::TTermination::SetSwitch				(IValue& value, TBlock& block_default)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TIndirectJumpInstruction&		TBlock::TTermination::SetIndirectJump		(IValue& block_address)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TUnreachableInstruction&		TBlock::TTermination::SetUnreachable		()
				{
					CL3_NOT_IMPLEMENTED;
				}

				/*************************************************************************************/

				CLASS		TFunction::TFunction	(TModule* module, const types::typeinfo::TRTTI* return_type) : TBlock(this), module(module), return_type(return_type)
				{
				}

				CLASS		TFunction::TFunction	(TModule* module, const TFunction& other) : TBlock(this), module(module)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS		TFunction::~TFunction	()
				{
					{
						const usys_t n = parameters.Count();
						for(usys_t i = 0; i < n; i++)
							delete parameters[i];
					}
					{
						const usys_t n = temporaries.Count();
						for(usys_t i = 0; i < n; i++)
							delete temporaries[i];
					}
					{
						const usys_t n = blocks.Count();
						for(usys_t i = 0; i < n; i++)
							delete blocks[i];
					}
				}

				TFunction::TParameter&
							TFunction::AddParameter	(const types::typeinfo::TRTTI* datatype, IValue* default_value)
				{
					return AddToListAndReturnValue(parameters, new TParameter(this, datatype, default_value));
				}

				TTemporary&	TFunction::AddTemporary	(IValue& source)
				{
					return AddToListAndReturnValue(temporaries, new TTemporary(this, &source));
				}

				TBlock&		TFunction::AddBlock		()
				{
					return AddToListAndReturnValue(blocks, new TBlock(this));
				}

				/*************************************************************************************/

				CLASS		TFunction::TParameter::TParameter	(TFunction* function, const types::typeinfo::TRTTI* datatype, IValue* default_value) : IValue(datatype), function(function), default_value(default_value)
				{
				}

				TFunction::TParameter&	TFunction::TParameter::CopyTo	(TBlock* block_new) const
				{
					//return block_new->Function().AddParameter(datatype, default_value->Lookup
					CL3_NOT_IMPLEMENTED;	//	TODO: requires "module.Lookup(value)"
				}

				/*************************************************************************************/

				CLASS		TModule::TModule		()
				{
				}

				CLASS		TModule::TModule		(const TModule& other)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS		TModule::~TModule		()
				{
					{
						const usys_t n = functions.Count();
						for(usys_t i = 0; i < n; i++)
							delete functions[i];
					}
					{
						const usys_t n = variables.Count();
						for(usys_t i = 0; i < n; i++)
							delete variables[i];
					}
					{
						const usys_t n = literals.Count();
						for(usys_t i = 0; i < n; i++)
							delete literals[i];
					}
				}

				TFunction&			TModule::AddFunction	(const types::typeinfo::TRTTI* return_type)
				{
					return AddToListAndReturnValue(functions, new TFunction(this, return_type));
				}

				TGenericVariable&	TModule::AddVariable	(const types::typeinfo::TRTTI* datatype, const void* value_init)
				{
					return AddToListAndReturnValue(variables, new TGenericVariable(this, datatype, value_init));
				}

				TGenericLiteral&	TModule::AddLiteral		(const types::typeinfo::TRTTI* datatype, const void* value)
				{
					return AddToListAndReturnValue(literals, new TGenericLiteral(this, datatype, value));
				}

				/*************************************************************************************/
			}
		}
	}
}
