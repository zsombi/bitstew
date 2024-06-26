/*
 * Copyright (C) 2024 bitWelder
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see
 * <http://www.gnu.org/licenses/>
 */

#include "utils/domain_test_environment.hpp"

#include <stew/stew.hpp>
#include <stew/meta/metadata/factory.hpp>
#include <stew/meta/object.hpp>
#include <stew/meta/object_extensions/object_extension.hpp>

namespace
{

class FunctionExtension : public stew::ObjectExtension
{
public:
    using FunctionType = std::function<void(FunctionExtension&)>;
    explicit FunctionExtension(std::string_view name) :
        stew::ObjectExtension(name)
    {
        m_function = [](auto&)
        {
            STEW_LOG_INFO("default");
        };
    }

    STEW_CLASS("FunctionExtension", FunctionExtension, stew::ObjectExtension)
    {
    };

    static auto create(std::string_view name)
    {
        return std::make_shared<FunctionExtension>(name);
    }

    void setFunction(FunctionType function)
    {
        m_function = std::move(function);
    }

protected:
    stew::ReturnValue runOverride(stew::PackagedArguments) final
    {
        if (m_function)
        {
            m_function(*this);
        }
        return {};
    }

    FunctionType m_function;
};


class ObjectExtensionTestBase : public DomainTestEnvironment
{
protected:
    stew::ObjectPtr testObject;
    std::shared_ptr<FunctionExtension> functor;

    void SetUp() override
    {
        // Single threaded
        initializeDomain(false, true);
        stew::Library::instance().objectFactory()->registerMetaClass(FunctionExtension::getStaticMetaClass());

        testObject = stew::Object::create("test");
        functor = FunctionExtension::create("functor");
        testObject->addExtension(functor);
    }
};
using ObjectExtensionTests = ObjectExtensionTestBase;

}


TEST_F(ObjectExtensionTests, hasOwner)
{
    EXPECT_EQ(testObject, functor->getObject());
    EXPECT_EQ(functor, testObject->findExtension("functor"));
}

TEST_F(ObjectExtensionTests, customExtension)
{
    EXPECT_CALL(*m_mockPrinter, log("default"));
    testObject->invoke("functor");
}

TEST_F(ObjectExtensionTests, changeFunction)
{
    auto lambda = [](FunctionExtension& self)
    {
        STEW_LOG_INFO(self.getObject()->getName() << " " << self.getName() << " with custom lambda");
    };
    functor->setFunction(lambda);
    EXPECT_CALL(*m_mockPrinter, log("test functor with custom lambda"));
    testObject->invoke("functor");
}
