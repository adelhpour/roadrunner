/*
 * EvalInitialConditionsCodeGen.cpp
 *
 *  Created on: Jun 29, 2013
 *      Author: andy
 */

#include "EvalInitialConditionsCodeGen.h"
#include "LLVMException.h"
#include "ASTNodeCodeGen.h"
#include "InitialValueSymbolResolver.h"
#include "rrLogger.h"
#include <sbml/math/ASTNode.h>
#include <sbml/math/FormulaFormatter.h>
#include <Poco/Logger.h>


using namespace libsbml;
using namespace llvm;
using namespace std;


namespace rr
{

const char* EvalInitialConditionsCodeGen::FunctionName = "evalInitialConditions";

EvalInitialConditionsCodeGen::EvalInitialConditionsCodeGen(
        const ModelGeneratorContext &mgc) :
        CodeGenBase<EvalInitialConditions_FunctionPtr>(mgc),
        initialValueResolver(model, dataSymbols, modelSymbols, builder)
{
}

EvalInitialConditionsCodeGen::~EvalInitialConditionsCodeGen()
{
}

Value* EvalInitialConditionsCodeGen::codeGen()
{
    Value *modelData = 0;

    BasicBlock *basicBlock = codeGenVoidModelDataHeader(FunctionName, modelData);

    Log(lInfo) << "boundarySpecies: \n";
    for (SymbolForest::ConstIterator i = modelSymbols.getInitialValues().boundarySpecies.begin();
            i != modelSymbols.getInitialValues().boundarySpecies.end(); i++)
    {
        char* formula = SBML_formulaToString(i->second);
        Log(lInfo) << "\t" << i->first << ": " << formula << "\n";
        free(formula);
    }

    ModelDataStoreSymbolResolver modelDataResolver(modelData, model,
            modelSymbols, dataSymbols, builder, initialValueResolver);

    codeGenSpecies(modelDataResolver);

    codeGenCompartments(modelDataResolver);

    codeGenStoichiometry(modelData, modelDataResolver);

    codeGenParameters(modelDataResolver);

    builder.CreateRetVoid();

    return verifyFunction();
}

void EvalInitialConditionsCodeGen::codeGenSpecies(
        ModelDataStoreSymbolResolver& modelDataResolver)
{
    {
        vector<string> floatingSpecies = dataSymbols.getFloatingSpeciesIds();

        for (vector<string>::const_iterator i = floatingSpecies.begin();
                i != floatingSpecies.end(); i++)
        {
            const string& id = *i;

            if (!dataSymbols.hasAssignmentRule(id))
            {
                modelDataResolver.storeSymbolValue(id,
                        initialValueResolver.loadSymbolValue(id));
            }
        }
    }

    {
        vector<string> boundarySpecies = dataSymbols.getBoundarySpeciesIds();

        for (vector<string>::const_iterator i = boundarySpecies.begin();
                i != boundarySpecies.end(); i++)
        {
            const string& id = *i;

            if (!dataSymbols.hasAssignmentRule(id))
            {
                modelDataResolver.storeSymbolValue(id,
                        initialValueResolver.loadSymbolValue(id));
            }
        }
    }
}


void EvalInitialConditionsCodeGen::codeGenStoichiometry(
        llvm::Value *modelData, ModelDataStoreSymbolResolver& modelDataResolver)
{
    ModelDataIRBuilder modelDataBuilder(modelData, dataSymbols,
                builder);
    ASTNodeCodeGen astCodeGen(builder, initialValueResolver);

    Log(lInfo) << "reactions: ";
    vector<string> ids = dataSymbols.getReactionIds();
    for (int i = 0; i < ids.size(); i++)
    {
        Log(lInfo) << ids[i] << ", ";
    }
    Log(lInfo) << "\n";

    Value *stoichEP = modelDataBuilder.createGEP(Stoichiometry);
    Value *stoich = builder.CreateLoad(stoichEP, "stoichiometry");

    list<LLVMModelDataSymbols::SpeciesReferenceInfo> stoichEntries =
            dataSymbols.getStoichiometryIndx();

    for (list<LLVMModelDataSymbols::SpeciesReferenceInfo>::iterator i =
            stoichEntries.begin(); i != stoichEntries.end(); i++)
    {
        LLVMModelDataSymbols::SpeciesReferenceInfo nz = *i;
        const ASTNode *node = modelSymbols.createStoichiometryNode(nz.row, nz.column);
        char* formula = SBML_formulaToString(node);
        Log(lInfo) << "\t{" << nz.row << ", " << nz.column << "} : " << formula
                << "\n";
        free(formula);

        // createCSRMatrixSetNZ(llvm::Value *csrPtr, llvm::Value *row,
        // llvm::Value *col, llvm::Value *value, const char* name = 0);

        Value *stoichValue = astCodeGen.codeGen(node);

        delete node;

        // species references may be defined by rate rules, so set the
        // initial value here. In this case, data is duplicated between the
        // rate rules vector and the CSR sparse matrix. (only occurs once
        // in 1100 tests)
        if (!nz.id.empty() && dataSymbols.hasRateRule(nz.id))
        {
            modelDataBuilder.createRateRuleValueStore(nz.id, stoichValue);
        }

        Value *row = ConstantInt::get(Type::getInt32Ty(context), nz.row, true);
        Value *col = ConstantInt::get(Type::getInt32Ty(context), nz.column, true);
        ModelDataIRBuilder::createCSRMatrixSetNZ(builder, stoich, row, col, stoichValue);

    }
}

void EvalInitialConditionsCodeGen::codeGenCompartments(
        ModelDataStoreSymbolResolver& modelDataResolver)
{
    vector<string> compartments = dataSymbols.getCompartmentIds();

    for (vector<string>::const_iterator i = compartments.begin();
            i != compartments.end(); i++)
    {
        const string& id = *i;

        if (!dataSymbols.hasAssignmentRule(id))
        {
            modelDataResolver.storeSymbolValue(id,
                    initialValueResolver.loadSymbolValue(id));
        }
    }
}

void EvalInitialConditionsCodeGen::codeGenParameters(
        ModelDataStoreSymbolResolver& modelDataResolver)
{
    vector<string> globalParameters = dataSymbols.getGlobalParameterIds();

    for (vector<string>::const_iterator i = globalParameters.begin();
            i != globalParameters.end(); i++)
    {
        const string& id = *i;

        if (!dataSymbols.hasAssignmentRule(id))
        {
            modelDataResolver.storeSymbolValue(id,
                    initialValueResolver.loadSymbolValue(id));
        }
    }
}

} /* namespace rr */
