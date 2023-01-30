#pragma once

struct ID3D11Buffer;
struct D3D11_SUBRESOURCE_DATA;
namespace SE
{
    namespace Helper
    {
        namespace BufferHelper
        {
            void MapBuffer(ID3D11Buffer* aBuffer, void* aBufferData, size_t aSizeOfBufferData);
            template <class IBufferData>
            void MapBuffer(ID3D11Buffer* aBuffer, IBufferData* aBufferData)
            {
                MapBuffer(aBuffer, aBufferData, sizeof(IBufferData));
            }

            /// <summary>
            /// </summary>
            /// <param name="aUsage">Default is D3D11_USAGE_DYNAMIC (2)</param>
            /// <param name="aBindFlag">Default is D3D11_BIND_CONSTANT_BUFFER (4)</param>
            /// <param name="aCpuAccessFlag">Default is D3D11_CPU_ACCESS_WRITE (65536)</param>
            void CreateBuffer(
                ID3D11Buffer** aBuffer,
                size_t aSizeOfBufferData,
                int aUsage = 2,
                int aBindFlag = 4,
                int aCpuAccessFlag = 0x10000,
                D3D11_SUBRESOURCE_DATA* const& someSubresourceData = nullptr
            );
            /// <summary>
            /// </summary>
            /// <param name="aUsage">If omitted, D3D11_USAGE_DYNAMIC will be used</param>
            /// <param name="aBindFlag">If omitted, D3D11_BIND_CONSTANT_BUFFER will be used</param>
            /// <param name="aCpuAccessFlag">If omitted, D3D11_CPU_ACCESS_WRITE will be used</param>
            template <class IBufferData>
            void CreateBuffer(
                ID3D11Buffer** aBuffer,
                int aUsage = 2,
                int aBindFlag = 4,
                int aCpuAccessFlag = 0x10000,
                D3D11_SUBRESOURCE_DATA* const& someSubresourceData = nullptr
            )
            {
                CreateBuffer(aBuffer, sizeof(IBufferData), aUsage, aBindFlag, aCpuAccessFlag, someSubresourceData);
            }
        }
    }
}
