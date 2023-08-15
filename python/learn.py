import torch
from ml import TrainingData, RLCTransformer
import pickle
from random import shuffle
from command_line import load_dataset, load_network
import argparse


def byte_arrays_to_training_data(byte_arrays: [[bytes]], score: [int]):
    train_count = (len(byte_arrays) // 10) * 7
    train_set = byte_arrays[0:train_count]
    train_result = score[0:train_count]
    eval_count = (len(byte_arrays) // 10) * 2
    val_set = byte_arrays[train_count : train_count + eval_count]
    val_result = score[train_count : train_count + eval_count]
    test_set = byte_arrays[train_count + eval_count :]
    test_result = score[train_count : train_count + eval_count]

    train = torch.IntTensor(train_set)
    train_result = torch.FloatTensor(train_result)
    val = torch.IntTensor(val_set)
    val_result = torch.FloatTensor(val_result)
    test = torch.IntTensor(test_set)
    test_result = torch.FloatTensor(test_result)

    return TrainingData.from_raw_bytes(
        train, train_result, val, val_result, test, test_result
    )


# returns table of expected scores [sequence_lenght, num_tokens]
def actions_and_scores_to_table(actions_and_scores):
    table = [[0 for x in range(258)] for x in range(4)]
    for (action, score) in actions_and_scores:
        for i, byte in enumerate(action):
            table[i + 1][byte] = max(table[i + 1][byte], score)
    table[0][256] = 100
    return table


def load_and_parse_dataset(path):
    games = []
    scores = []
    dataset = load_dataset(path)
    for (state, actions_and_scores) in dataset:
        probabilities = actions_and_scores_to_table(actions_and_scores)
        scores.append(probabilities)
        games.append(state)

    zipped = list(zip(games, scores))
    shuffle(zipped)
    (games2, scores2) = zip(*zipped)

    training_data = byte_arrays_to_training_data(games2, scores2)
    return training_data


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        "solve", description="runs a action of the simulation"
    )
    parser.add_argument("dataset", type=str)
    parser.add_argument("--network", "-n", type=str, default="")
    parser.add_argument("--output", "-o", type=str, default="")
    parser.add_argument("--epochs", "-e", default=10, type=int)

    args = parser.parse_args()

    training_data = load_and_parse_dataset(args.dataset)

    print("training data size: ", training_data.train_data.size())
    transformer = load_network(
        args.network, training_data.ntokens, training_data.device
    )

    transformer.train_from_dataset(training_data, epochs=args.epochs)
    if args.output != "":
        torch.save(transformer, args.output)
