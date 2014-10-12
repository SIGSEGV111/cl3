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

namespace	cl3
{
	namespace	system
	{
		namespace	compiler
		{
			namespace	jit
			{
				/*************************************************************************************/

				CLASS	IInstruction::~IInstruction	()
				{
					CL3_NOT_IMPLEMENTED;
				}

				/*************************************************************************************/

				CLASS	IValue::IValue		(const types::typeinfo::TRTTI*)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS	IValue::~IValue		()
				{
					CL3_NOT_IMPLEMENTED;
				}

				const types::typeinfo::TRTTI*
						IValue::Datatype	() const
				{
					CL3_NOT_IMPLEMENTED;
				}

				void	IValue::Datatype	(const types::typeinfo::TRTTI*)
				{
					CL3_NOT_IMPLEMENTED;
				}

				/*************************************************************************************/

				CLASS	ILiteral::ILiteral	(TModule* module, const types::typeinfo::TRTTI* datatype) : IValue(datatype)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS	TGenericLiteral::TGenericLiteral	(TModule* module, const types::typeinfo::TRTTI* datatype, const void* value_init) : ILiteral(module, datatype)
				{
					CL3_NOT_IMPLEMENTED;
				}

				/*************************************************************************************/

				CLASS	IVariable::IVariable	(TModule* module, const types::typeinfo::TRTTI* datatype) : IValue(datatype)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS	TGenericVariable::TGenericVariable	(TModule* module, const types::typeinfo::TRTTI* datatype, const void* value_init) : IVariable(module, datatype)
				{
					CL3_NOT_IMPLEMENTED;
				}

				/*************************************************************************************/

				CLASS	TTemporary::TTemporary	(TFunction* function, IValue* source) : IValue(source->Datatype())
				{
					CL3_NOT_IMPLEMENTED;
				}

				/*************************************************************************************/

				CLASS	TBlock::TBlock	(TFunction* function) : termination(this)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS	TBlock::TBlock	(TFunction* function, const TBlock& other) : termination(this)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS	TBlock::~TBlock	()
				{
					CL3_NOT_IMPLEMENTED;
				}

				TAddInstruction&		TBlock::AddAdd			(IValue& op1, IValue& op2)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TSubInstruction&		TBlock::AddSub			(IValue& op1, IValue& op2)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TMulInstruction&		TBlock::AddMul			(IValue& op1, IValue& op2)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TDivInstruction&		TBlock::AddDiv			(IValue& op1, IValue& op2)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TModInstruction&		TBlock::AddMod			(IValue& op1, IValue& op2)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TShiftInstruction&		TBlock::AddShift		(IValue& value, IValue& times, EDirection direction)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TAndInstruction&		TBlock::AddAnd			(IValue& op1, IValue& op2)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TOrInstruction&			TBlock::AddOr			(IValue& op1, IValue& op2)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TXorInstruction&		TBlock::AddXor			(IValue& op1, IValue& op2)
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

				CLASS		TFunction::TFunction	(TModule* module) : TBlock(this)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS		TFunction::TFunction	(TModule* module, const TFunction& other) : TBlock(this)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS		TFunction::~TFunction	()
				{
					CL3_NOT_IMPLEMENTED;
				}

				TFunction::TParameter&
							TFunction::AddParameter	(const types::typeinfo::TRTTI* datatype, IValue* default_value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TTemporary&	TFunction::AddTemporary	(IValue& source)
				{
					CL3_NOT_IMPLEMENTED;
				}

				TBlock&		TFunction::AddBlock		()
				{
					CL3_NOT_IMPLEMENTED;
				}

				/*************************************************************************************/

				CLASS		TFunction::TParameter::TParameter	(TFunction* function, const types::typeinfo::TRTTI* datatype, IValue* default_value) : IValue(datatype)
				{
					CL3_NOT_IMPLEMENTED;
				}

				/*************************************************************************************/

				CLASS		TModule::TModule		()
				{
				}

				CLASS		TModule::TModule		(const TModule&)
				{
					CL3_NOT_IMPLEMENTED;
				}

				CLASS		TModule::~TModule		()
				{
				}

				TFunction&	TModule::AddFunction	(const types::typeinfo::TRTTI* return_type)
				{
					CL3_NOT_IMPLEMENTED;
				}

				IVariable&	TModule::AddVariable	(const types::typeinfo::TRTTI* datatype, const void* value_init)
				{
					CL3_NOT_IMPLEMENTED;
				}

				ILiteral&	TModule::AddLiteral		(const types::typeinfo::TRTTI* datatype, const void* value)
				{
					CL3_NOT_IMPLEMENTED;
				}

				/*************************************************************************************/
			}
		}
	}
}
