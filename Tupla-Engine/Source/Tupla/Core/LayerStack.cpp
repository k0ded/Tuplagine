#include "tgpch.h"
#include "Tupla/Core/LayerStack.h"

namespace Tupla
{
    LayerStack::~LayerStack()
    {
        for (const auto layer : m_Layers)
        {
            layer->OnDetach();
            delete layer;
        }
    }

    void LayerStack::PushLayer(Tupla::Layer* layer)
    {
        m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
        m_LayerInsertIndex++;
    }

    void LayerStack::PushOverlayLayer(Layer* layer)
    {
        m_Layers.emplace_back(layer);
    }

    void LayerStack::PopLayer(Layer* layer)
    {
        const auto last = m_Layers.begin() + m_LayerInsertIndex;
        const auto foundLoc = std::find(m_Layers.begin(), last, layer);

        if(foundLoc != last)
        {
            layer->OnDetach();
            m_Layers.erase(foundLoc);
            --m_LayerInsertIndex;
        }
    }

    void LayerStack::PopOverlayLayer(Layer* layer)
    {
        const auto foundLoc = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), layer);

        if(foundLoc != m_Layers.end())
        {
            layer->OnDetach();
            m_Layers.erase(foundLoc);
        }
    }
}
