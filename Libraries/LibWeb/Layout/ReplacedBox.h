/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <LibWeb/DOM/Element.h>
#include <LibWeb/Layout/Box.h>

namespace Web::Layout {

class ReplacedBox : public Box {
public:
    ReplacedBox(DOM::Document&, DOM::Element&, NonnullRefPtr<CSS::StyleProperties>);
    virtual ~ReplacedBox() override;

    const DOM::Element& dom_node() const { return downcast<DOM::Element>(*Node::dom_node()); }
    DOM::Element& dom_node() { return downcast<DOM::Element>(*Node::dom_node()); }

    virtual bool is_replaced() const final { return true; }

    bool has_intrinsic_width() const { return m_has_intrinsic_width; }
    bool has_intrinsic_height() const { return m_has_intrinsic_height; }
    bool has_intrinsic_ratio() const { return m_has_intrinsic_ratio; }

    float intrinsic_width() const { return m_intrinsic_width; }
    float intrinsic_height() const { return m_intrinsic_height; }
    float intrinsic_ratio() const { return m_intrinsic_ratio; }

    void set_has_intrinsic_width(bool has) { m_has_intrinsic_width = has; }
    void set_has_intrinsic_height(bool has) { m_has_intrinsic_height = has; }
    void set_has_intrinsic_ratio(bool has) { m_has_intrinsic_ratio = has; }

    void set_intrinsic_width(float width) { m_intrinsic_width = width; }
    void set_intrinsic_height(float height) { m_intrinsic_height = height; }
    void set_intrinsic_ratio(float ratio) { m_intrinsic_ratio = ratio; }

    float calculate_width() const;
    float calculate_height() const;

    virtual void prepare_for_replaced_layout() { }

protected:
    virtual void split_into_lines(Layout::BlockBox& container, LayoutMode) override;

private:
    virtual const char* class_name() const override { return "ReplacedBox"; }

    bool m_has_intrinsic_width { false };
    bool m_has_intrinsic_height { false };
    bool m_has_intrinsic_ratio { false };
    float m_intrinsic_width { 0 };
    float m_intrinsic_height { 0 };
    float m_intrinsic_ratio { 0 };
};

}

AK_BEGIN_TYPE_TRAITS(Web::Layout::ReplacedBox)
static bool is_type(const Web::Layout::Node& layout_node) { return layout_node.is_replaced(); }
AK_END_TYPE_TRAITS()
