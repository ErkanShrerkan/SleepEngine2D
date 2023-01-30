#include "pch.h"
#include "TextureHelper.h"
#include <DirectXTK\DDSTextureLoader.h> // .dds
#include <DirectXTK\WICTextureLoader.h> // .png .jpg etc
#include "Engine.h" // device
#include <CommonUtilities\Algorithm\Hash.hpp>

namespace SE
{
	namespace Helper
	{
		namespace TextureHelper
		{
			bool LoadShaderResourceView(ID3D11ShaderResourceView** aShaderResourceView, const std::string& aFilePath)
			{
				return TextureBank::GetInstance().GetResource(aShaderResourceView, aFilePath);
			}

			bool TextureBank::CreateResource(ID3D11ShaderResourceView** aShaderResourceView, const std::string& aFilePath)
			{
				// TODO: Maybe use a CTextureFactory instead
				ID3D11Device* device = CEngine::GetInstance()->GetDXDevice();

				size_t dot = aFilePath.find_last_of('.');
				if (dot == std::string::npos)
				{
					*aShaderResourceView = nullptr;
					return false;
				}

				std::string extension = aFilePath.substr(dot);
				std::wstring wFilePath(aFilePath.begin(), aFilePath.end());

				if (extension == ".dds")
				{
					DirectX::CreateDDSTextureFromFile(device, wFilePath.c_str(), nullptr, aShaderResourceView);
				}
				else
				{
					/* Warning, texture is not dds */
					pwarn("Texture \"%s\" is not a .dds", std::string(aFilePath).c_str());
					DirectX::CreateWICTextureFromFile(device, wFilePath.c_str(), nullptr, aShaderResourceView);
				}

				// TODO: Warn about error loading shader resource view

				if (*aShaderResourceView == nullptr)
				{
					return false;
				}

				myMutex.lock();
				resources[aFilePath] = *aShaderResourceView;
				printf("created texture %s\n", aFilePath.c_str());
				myMutex.unlock();

				return true;
			}

			bool TextureBank::GetResource(ID3D11ShaderResourceView** aShaderResourceView, const std::string& aFilePath)
			{
				myMutex.lock();
				std::string cleanName(aFilePath);
				std::replace(cleanName.begin(), cleanName.end(), '\\', '/');
#pragma warning(disable:4244)
				// '=': conversion from 'int' to 'char', possible loss of data
				std::string lowerCasePath = cleanName;
				std::transform(cleanName.begin(), cleanName.end(), lowerCasePath.begin(), std::tolower);
#pragma warning(default:4244)
				bool exists = resources.find(lowerCasePath) != resources.end();
				myMutex.unlock();
				if (exists)
				{
					*aShaderResourceView = resources.at(lowerCasePath);
					printf("loaded texture %s\n", lowerCasePath.c_str());
					return true;
				}
				else
				{
					return CreateResource(aShaderResourceView, lowerCasePath);
				}
			}
			void TextureBank::Release()
			{
				//for (auto& tex : resources)
				//{
				//	tex.second->Release();
				//}
			}
		}
	}
}
