#pragma once

//----cereal----//
#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/unordered_map.hpp>

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#include <vector>
#include <string>
#include <fbxsdk.h>
#include <unordered_map>

#include "constants.h"

namespace DirectX
{
    template<class T>
    void serialize(T& archive, DirectX::XMFLOAT2& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y)
        );
    }

    template<class T>
    void serialize(T& archive, DirectX::XMFLOAT3& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z)
        );
    }

    template<class T>
    void serialize(T& archive, DirectX::XMFLOAT4& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z),
            cereal::make_nvp("w", v.w)
        );
    }

    template<class T>
    void serialize(T& archive, DirectX::XMFLOAT4X4& m)
    {
        archive(
            cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12),
            cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
            cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22),
            cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
            cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32),
            cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
            cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42),
            cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
        );
    }
}

struct scene
{
    struct node
    {
        uint64_t unique_id{ 0 };
        std::string name;
        FbxNodeAttribute::EType attribute{ FbxNodeAttribute::EType::eUnknown };
        int64_t parent_index{ -1 };

        template<class T>
        void serialize(T& archive) { archive(unique_id, name, attribute, parent_index); };
    };
    std::vector<node> nodes;
    int64_t indexof(uint64_t unique_id) const
    {
        int64_t index{ 0 };
        for (const node& node : nodes)
        {
            if (node.unique_id == unique_id)
            {
                return index;
            }
            ++index;
        }
        return -1;
    }
    template<class T>
    void serialize(T& archive) { archive(nodes); };
};

struct skeleton
{
    struct bone
    {
        uint64_t unique_id{ 0 };
        std::string name;
        // 'parent_index' is index that refers to the parent bone's position in the array that contains itself.
        int64_t parent_index{ -1 }; // -1 : the bone is orphan
        // 'node_index' is an index that refers to the node array of the scene.
        int64_t node_index{ 0 };
        // 'offset_transform' is used to convert from model(mesh) space to bone(node) scene.
        DirectX::XMFLOAT4X4 offset_transform{
            1, 0, 0, 0,    0, 1, 0, 0,    0, 0, 1, 0,    0, 0, 0, 1
        };

        bool is_orphan() const { return parent_index < 0; };

        template<class T>
        void serialize(T& archive) { archive(unique_id, name, parent_index, node_index, offset_transform); }
    };
    std::vector<bone> bones;
    int64_t indexof(uint64_t unique_id) const
    {
        int64_t index{ 0 };
        for (const bone& bone : bones)
        {
            if (bone.unique_id == unique_id)
            {
                return index;
            }
            ++index;
        }
        return -1;
    }
    template<class T>
    void serialize(T& archive) { archive(bones); };
};

struct animation
{
    std::string name;
    float sampling_rate{ 0 };

    struct keyframe
    {
        struct node
        {
            // 'global_transform' is used to convert from local space of node to global space of scene.
            DirectX::XMFLOAT4X4 global_transform{
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
            };
            // The transformation data of a node includes its translation, rotation and scaling vectors
            // with respect to its parent.
            DirectX::XMFLOAT3 scaling{ 1, 1, 1 };
            DirectX::XMFLOAT4 rotation{ 0, 0, 0, 1 }; // Rotation quaternion
            DirectX::XMFLOAT3 translation{ 0, 0, 0 };

            template<class T>
            void serialize(T& archive) { archive(global_transform, scaling, rotation, translation); };
        };
        std::vector<node> nodes;

        template<class T>
        void serialize(T& archive) { archive(nodes); };
    };
    std::vector<keyframe> sequence;

    template<class T>
    void serialize(T& archive) { archive(name, sampling_rate, sequence); };
};

class SkinnedMesh
{
public:
    //--------< コンストラクタ/関数等 >--------//
    SkinnedMesh(ID3D11Device* device, const char* fbx_filename, bool triangulate = false, float sampling_rate = 0);
    virtual ~SkinnedMesh() = default;
    void render(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color, float threshold = 0,
        const DirectX::XMFLOAT4& emissive_color = DirectX::XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ));
    // アニメーション
    void play_animation(int animation_index, bool is_loop = false, float blend_seconds = 0.3f);
    void pause_animation() { anim_para.stop_animation = true; }
    void progress_animation() { anim_para.stop_animation = false; }
    void update_animation(float elapsed_time);
    bool end_of_animation() const { return anim_para.end_of_animation; }

    void find_bone_by_name(const DirectX::XMFLOAT4X4& world, std::string name, DirectX::XMFLOAT3& pos, DirectX::XMFLOAT3& up);
private:
    //--------< 構造体 >--------//
    static const int MAX_BONE_INFLUENCES{ 4 };
    static const int MAX_BONES{ 256 };
    struct GeometryConstants
    {
        DirectX::XMFLOAT4X4 world;
        DirectX::XMFLOAT4 material_color;
        DirectX::XMFLOAT4X4 bone_transforms[MAX_BONES]{
            {1,0,0,0,  0,1,0,0,  0,0,1,0,  0,0,0,1}
        };
        DirectX::XMFLOAT4 emissive_color{ 1,1,1,1 }; // xyz:emissive_color w:emissive_strength
        DirectX::XMFLOAT4 dissolve_threshold{ 0,0,0,0 };
    };
    std::unique_ptr<Constants<GeometryConstants>> geometry_constants;
    // FBXファイルからメッシュが使用するマテリアル情報（色・テクスチャ）を抽出する
    struct material
    {
        uint64_t unique_id{ 0 };
        std::string name;

        DirectX::XMFLOAT4 Ka{ 0.2f, 0.2f, 0.2f, 1.0f };
        DirectX::XMFLOAT4 Kd{ 0.8f, 0.8f, 0.8f, 1.0f };
        DirectX::XMFLOAT4 Ks{ 1.0f, 1.0f, 1.0f, 1.0f };

        static const int TEXTURE_COUNT = 8;
        std::string texture_filenames[TEXTURE_COUNT];
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[TEXTURE_COUNT];

        template<class T>
        void serialize(T& archive) { archive(unique_id, name, Ka, Kd, Ks, texture_filenames); }
    };
    std::unordered_map<uint64_t, material> materials;
    struct anim_Parameters
    {
        int current_anim_index{ 0 };               // 現在のアニメーションのインデックス
        int old_anim_index{ 0 };                   // ひとつ前ののアニメーションのインデックス
        int frame_index{ 0 };                      // 再生中のアニメーションのフレーム
        float animation_tick{ 0 };                 // フレームの中の再生時間
        bool do_loop{ false };                     // ループ再生するか
        bool stop_animation{ false };              // アニメーションの停止
        bool end_of_animation{ false };            // アニメーションが1ループ再生したか
        float animation_blend_time = 0.0f;         // アニメーション間の補完時間
        float animation_blend_seconds = 0.0f;      // アニメーション間の補完割合

        animation animation{};
        animation::keyframe current_keyframe{};
    } anim_para;
public:
    struct vertex
    {
        DirectX::XMFLOAT3 position{};
        DirectX::XMFLOAT3 normal{};
        DirectX::XMFLOAT4 tangent{};
        DirectX::XMFLOAT2 texcoord{};
        float bone_weights[MAX_BONE_INFLUENCES]{ 1,0,0,0 };
        uint32_t bone_indices[MAX_BONE_INFLUENCES]{};

        template<class T>
        void serialize(T& archive) { archive(position, normal, tangent, texcoord, bone_weights, bone_indices); }
    };
    struct mesh
    {
        uint64_t unique_id{ 0 };
        std::string name;
        // 'node_index' is an index that refers to the node array of the scene.
        int64_t node_index{ 0 };

        std::vector<vertex> vertices;
        std::vector<uint32_t> indices;
        // マテリアル単位で描画を行う
        struct subset
        {
            uint64_t material_unique_id{ 0 };
            std::string material_name;
            uint32_t start_index_location{ 0 };
            uint32_t index_count{ 0 };

            template<class T>
            void serialize(T& archive) { archive(material_unique_id, material_name, start_index_location, index_count); }
        };
        std::vector<subset> subsets;

        template<class T>
        void serialize(T& archive)
        {
            archive(unique_id, name, node_index, subsets, default_global_transform,
                bind_pose, bounding_box, vertices, indices);
        }
        DirectX::XMFLOAT4X4 default_global_transform{
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1 };
    private:
        skeleton bind_pose;
        Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
        DirectX::XMFLOAT3 bounding_box[2]
        {
            { +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX },
            { -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX }
        };
        friend class SkinnedMesh;
    };
private:
    //--------< 変数 >--------//
    std::vector<mesh> meshes;
    std::vector<animation> animation_clips;
private:
    void fetch_meshes(FbxScene* fbx_scene, std::vector<mesh>& meshes);
    void fetch_materials(const char* fbx_filename, FbxScene* fbx_scene, std::unordered_map<uint64_t, material>& materials);
    // FBXメッシュからバインドポーズの情報を抽出する
    void fetch_skeleton(FbxMesh* fbx_mesh, skeleton& bind_pose);
    void create_com_objects(ID3D11Device* device, const char* fbx_filename);
    // FBXシーンからアニメーションの情報を抽出する
    void fetch_animations(FbxScene* fbx_scene, std::vector<animation>& animation_clips,
        float sampling_rate /*If this value is 0, the animation data will be sampled at the default frame rate.*/);
    // アニメーション関連
    void update_blend_animation(animation::keyframe& keyframe);
    bool append_animations(const char* animation_filename, float sampling_rate);
    void blend_animations(const animation::keyframe* keyframes[2], float factor, animation::keyframe& keyframe);
    // 固有IDからノードを取得
    FbxNode* find_fbxNode_by_uniqueId(FbxNode* fbx_node, uint64_t id);
    FbxNode* find_fbxNode_by_uniqueId(FbxScene* fbx_scene, uint64_t id);
protected:
    scene scene_view;
public:
    //--------<getter/setter>--------//
    const std::vector<mesh>& get_meshes() const { return meshes; }
    const anim_Parameters& get_anim_para() const { return anim_para; }
};