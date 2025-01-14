import abc

import pathlib


class Decoder(metaclass=abc.ABCMeta):
    """Predicts observable flips from detection event data.

    Custom decoders can be explained to sinter by inheriting from this class and
    implementing its methods in a way that call the custom decoder.

    Decoder classes MUST be serializable (e.g. via pickling), so that they can be given to
    worker processes when using python multiprocessing.
    """

    @abc.abstractmethod
    def decode_via_files(self,
                         *,
                         num_shots: int,
                         num_dets: int,
                         num_obs: int,
                         dem_path: pathlib.Path,
                         dets_b8_in_path: pathlib.Path,
                         obs_predictions_b8_out_path: pathlib.Path,
                         tmp_dir: pathlib.Path,
                       ) -> None:
        """Performs decoding by reading problems from, and writing solutions to, file paths.

        Args:
            num_shots: The number of times the circuit was sampled. The number of problems
                to be solved.
            num_dets: The number of detectors in the circuit. The number of detection event
                bits in each shot.
            num_obs: The number of observables in the circuit. The number of predicted bits
                in each shot.
            dem_path: The file path where the detector error model should be read from,
                e.g. using `stim.DetectorErrorModel.from_file`. The error mechanisms
                specified by the detector error model should be used to configure the
                decoder.
            dets_b8_in_path: The file path that detection event data should be read from.
                Note that the file may be a named pipe instead of a fixed size object.
                The detection events will be in b8 format (see
                https://github.com/quantumlib/Stim/blob/main/doc/result_formats.md ). The
                number of detection events per shot is available via the `num_dets`
                argument or via the detector error model at `dem_path`.
            obs_predictions_b8_out_path: The file path that decoder predictions must be
                written to. The predictions must be written in b8 format (see
                https://github.com/quantumlib/Stim/blob/main/doc/result_formats.md ). The
                number of observables per shot is available via the `num_obs` argument or
                via the detector error model at `dem_path`.
            tmp_dir: Any temporary files generated by the decoder during its operation MUST
                be put into this directory. The reason for this requirement is because
                sinter is allowed to kill the decoding process without warning, without
                giving it time to clean up any temporary objects. All cleanup should be done
                via sinter deleting this directory after killing the decoder.
        """
        pass
