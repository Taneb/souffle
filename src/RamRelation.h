/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2014, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file RamRelation.h
 *
 * Defines the class for ram relations
 ***********************************************************************/

#pragma once

#include "IODirectives.h"
#include "ParallelUtils.h"
#include "RamNode.h"
#include "RamTypes.h"
#include "RelationRepresentation.h"
#include "SymbolTable.h"
#include "Table.h"
#include "Util.h"

#include <list>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace souffle {

/**
 * A RAM Relation in the RAM intermediate representation.
 */
class RamRelation : public RamNode {
protected:
    /** Name of relation */
    const std::string name;

    /** Arity, i.e., number of attributes */
    const size_t arity;

    /** Name of attributes */
    const std::vector<std::string> attributeNames;

    /** Type of attributes */
    const std::vector<std::string> attributeTypeQualifiers;

    /** Data-structure representation */
    const RelationRepresentation representation;

public:
    RamRelation(const std::string name, const size_t arity, const std::vector<std::string> attributeNames,
            const std::vector<std::string> attributeTypeQualifiers,
            const RelationRepresentation representation)
            : RamNode(RN_Relation), name(std::move(name)), arity(arity),
              attributeNames(std::move(attributeNames)),
              attributeTypeQualifiers(std::move(attributeTypeQualifiers)), representation(representation) {
        assert(this->attributeNames.size() == arity || this->attributeNames.empty());
        assert(this->attributeTypeQualifiers.size() == arity || this->attributeTypeQualifiers.empty());
    }

    /** Get name */
    const std::string& getName() const {
        return name;
    }

    /** Get argument */
    const std::string getArg(uint32_t i) const {
        if (!attributeNames.empty()) {
            return attributeNames[i];
        }
        if (arity == 0) {
            return "";
        }
        return "c" + std::to_string(i);
    }

    /** Get Argument Type Qualifier */
    const std::string getArgTypeQualifier(uint32_t i) const {
        return (i < attributeTypeQualifiers.size()) ? attributeTypeQualifiers[i] : "";
    }

    const std::vector<std::string>& getAttributeTypeQualifiers() const {
        return attributeTypeQualifiers;
    }

    /** Is nullary relation */
    const bool isNullary() const {
        return arity == 0;
    }

    /** Relation representation type */
    const RelationRepresentation getRepresentation() const {
        return representation;
    }

    /** Is temporary relation (for semi-naive evaluation) */
    const bool isTemp() const {
        return name.at(0) == '@';
    }

    /* Get arity of relation */
    unsigned getArity() const {
        return arity;
    }

    /* Compare two relations via their name */
    bool operator<(const RamRelation& other) const {
        return name < other.name;
    }

    /* Print */
    void print(std::ostream& out) const override {
        out << name;
        if (arity > 0) {
            out << "(" << getArg(0) << ":" << attributeTypeQualifiers[0];
            for (unsigned i = 1; i < arity; i++) {
                out << ",";
                out << getArg(i) << ":" << attributeTypeQualifiers[i];
            }
            out << ")";
            out << " " << representation;
        } else {
            out << " nullary";
        }
    }

    /** Obtain list of child nodes */
    std::vector<const RamNode*> getChildNodes() const override {
        return std::vector<const RamNode*>();  // no child nodes
    }

    /** Create clone */
    RamRelation* clone() const override {
        RamRelation* res =
                new RamRelation(name, arity, attributeNames, attributeTypeQualifiers, representation);
        return res;
    }

    /** Apply mapper */
    void apply(const RamNodeMapper& map) override {}

protected:
    /** Check equality */
    bool equal(const RamNode& node) const override {
        assert(nullptr != dynamic_cast<const RamRelation*>(&node));
        const auto& other = static_cast<const RamRelation&>(node);
        return name == other.name && arity == other.arity && attributeNames == other.attributeNames &&
               attributeTypeQualifiers == other.attributeTypeQualifiers &&
               representation == other.representation && isTemp() == other.isTemp();
    }
};

/**
 * A RAM Relation in the RAM intermediate representation.
 */
class RamRelationReference : public RamNode {
public:
    RamRelationReference(const RamRelation* relation) : RamNode(RN_RelationReference), relation(relation) {
        assert(relation != nullptr && "null relation");
    }

    /** Get reference */
    const RamRelation* get() const {
        assert(relation != nullptr && "null relation");
        return relation;
    }

    /* Print */
    void print(std::ostream& out) const override {
        out << relation->getName();
    }

    /** Obtain list of child nodes */
    std::vector<const RamNode*> getChildNodes() const override {
        return std::vector<const RamNode*>();  // no child nodes
    }

    /** Create clone */
    RamRelationReference* clone() const override {
        auto* res = new RamRelationReference(relation);
        return res;
    }

    /** Apply mapper */
    void apply(const RamNodeMapper& map) override {}

protected:
    /** Name of relation */
    const RamRelation* relation;

    /** Check equality */
    bool equal(const RamNode& node) const override {
        assert(nullptr != dynamic_cast<const RamRelationReference*>(&node));
        const auto& other = static_cast<const RamRelationReference&>(node);
        return relation == other.relation;
    }
};

}  // end of namespace souffle
