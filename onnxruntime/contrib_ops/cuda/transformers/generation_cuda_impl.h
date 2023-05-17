// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <stdint.h>
#include <cuda_fp16.h>
#include <curand_kernel.h>

namespace onnxruntime {
namespace contrib {
namespace cuda {

void LaunchInitKernel(
    float* beam_scores,
    int batch_size,
    int num_beams,
    cudaStream_t stream);

template <typename T>
void LaunchAddProbsKernel(T* log_probs,
                          T* cum_log_probs,
                          const int batch_size,
                          const int num_beams,
                          const int vocab_size,
                          cudaStream_t stream);

template <typename T>
void LaunchLogitsProcessKernel(
    T* next_token_scores,
    const int* vocab_mask,
    const int* prefix_vocab_mask,
    int* presence_mask,
    float presence_penalty,
    float temperature,
    int batch_size,
    int num_beams,
    int vocab_size,
    int padded_vocab_size,
    int demote_token_id,
    int32_t* sequences,
    int max_sequence_length,
    int current_sequence_length,
    float repetition_penalty,
    int no_repeat_ngram_size,
    cudaStream_t stream);

void LaunchNextTokenKernel(const int64_t* next_token_indices,
                           int32_t* next_indices,
                           int32_t* next_tokens,
                           int batch_size,
                           int top_k,
                           int vocab_size,
                           cudaStream_t stream);

void LaunchUpdateGptKernel(const int32_t* old_mask_data,
                           int32_t* mask_data,
                           int32_t* next_positions,
                           int batch_beam_size,
                           int current_length,
                           cudaStream_t stream);

template <typename T>
void GetTempStorageSize(const T* d_keys_in,
                        const int* d_values_in,
                        int* d_offsets,
                        int num_items,
                        int num_segments,
                        cudaStream_t stream,
                        bool is_descending,
                        size_t& temp_storage_bytes);

void LaunchSetupParamsKernel(int* d_values_in,
                             int* d_offsets,
                             int batch_size,
                             int vocab_size,
                             cudaStream_t stream);

template <typename T>
void LaunchSortPairs(void* d_temp_storage,
                     size_t temp_storage_bytes,
                     const T* d_keys_in,
                     T* d_keys_out,
                     const int* d_values_in,
                     int* d_values_out,
                     int num_items,
                     int num_segments,
                     int* d_offsets,
                     cudaStream_t stream,
                     bool is_descending);

template <typename T>
void LaunchFilterLogitsKernel(float* d_sorted_logits_in,
                              const int* d_sorted_indices,
                              T* d_logits_in_out,
                              float top_p,
                              float filter_value,
                              int min_tokens_to_keep,
                              int batch_size,
                              int vocab_size,
                              cudaStream_t stream,
                              bool is_descending);

void TorchMultinomialKernelLauncher(float* d_input,
                                    float* d_sampled,
                                    int32_t* d_output,
                                    int batch_size,
                                    int vocab_size,
                                    int* d_presence_mask,
                                    cudaStream_t stream);

void UpdateDecoderMaskedMultiHeadAttentionCacheIndirection(int32_t* tgt_indir_cache,
                                                           const int32_t* src_indir_cache,
                                                           const int32_t* beam_ids,
                                                           int batch_size,
                                                           int beam_width,
                                                           int input_seq_length,
                                                           int max_seq_length,
                                                           int current_length,
                                                           cudaStream_t stream);

template <typename T>
void KeyCacheExpansionKernelLauncher(const T* key_cache,
                                     T* key_cache_expanded,
                                     int batch_size,
                                     int beam_width,
                                     int num_heads,
                                     int sequence_length,
                                     int max_seq_length,
                                     int head_size,
                                     cudaStream_t stream);

template <typename T>
void BufferExpansionKernelLauncher(const T* input,
                                   T* output,
                                   int batch_size,
                                   int beam_width,
                                   int chunk_size,
                                   cudaStream_t stream);

void LaunchCopyCrossQKSingleDecodeStep(
    cudaStream_t stream,
    float* cross_qk_buffer_data,
    float** qk_layer_pointers,
    int token_index,
    int batchxbeam,
    int num_layers,
    int num_heads,
    int cross_qk_layer_head_pair_count,
    const int* cross_qk_layer_head_pairs,
    int frames,
    int max_length);

void LaunchFinalizeCrossQK(
    cudaStream_t stream,
    int iteration_number,
    int context_decoding_len,
    int batch_size,
    int num_beams,
    int max_length,
    int cross_qk_layer_head_pair_count,
    const int* cross_qk_layer_head_pairs,
    int frames_of_k,
    const float* cross_qk_buffer_data,
    float* cross_qk_output,
    int num_return_sequences,
    const int* cache_indir_data,
    const int32_t* beam_indices);

void LaunchForceDecodingIds(
    float* beam_scores,
    const int batch_size,
    const int num_beams,
    const int vocab_size,
    const int32_t* force_ids,
    int id_len,
    int step,
    cudaStream_t stream);

template <typename T>
void LaunchSaveNoSpeechProbs(
    T* result_no_speech_probs, /* [batch]*/
    const float* probs,        /* [batch, num_beams, vocab_size]*/
    const int batch_size,
    const int num_beams,
    const int vocab_size,
    const int no_speech_token_id,
    cudaStream_t stream);

}  // namespace cuda
}  // namespace contrib
}  // namespace onnxruntime